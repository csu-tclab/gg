/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "net/crail.hh"

#include <thread>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>

#include "util/exception.hh"
#include "util/file_descriptor.hh"
#include "util/optional.hh"

using namespace std;

void CrailClient::upload_files( const std::vector<storage::PutRequest> & upload_requests,
                     const std::function<void( const storage::PutRequest & )> & success_callback) {
  const size_t thread_count = 1;
  const size_t batch_size = config_.max_batch_size;
  vector<thread> threads;
  for ( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    if ( thread_index < upload_requests.size() ) {
      threads.emplace_back(
        [&] ( const size_t index )
        {
          // try to connect to Crail server
          std::shared_ptr<CrailStore> crailStore;
          crailStore.reset(new CrailStore(config_.namenode_address, config_.port));

          std::cout << "\n[INFO] [upload_files] thread index [" << thread_index << "] " << "begin connect to crail";
          crailStore->Initialize();
          std::cout << "\n[INFO] [upload_files] thread index [" << thread_index << "] " << "connect to crail server end";
          // we can't check the connect result

          for ( size_t first_file_idx = index;
                first_file_idx < upload_requests.size();
                first_file_idx += thread_count * batch_size ) {

            size_t expected_responses = 0;

            for ( size_t file_id = first_file_idx;
                  file_id < min( upload_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & filename = upload_requests.at( file_id ).filename.string();
              const string & object_key = upload_requests.at( file_id ).object_key;

              cout << "\n[NOTICE] [upload_files] filename: " << filename;

              FILE *fp = fopen(filename.c_str(), "r");
              if (!fp) {
                cout << "could not open local file " << filename.c_str() << endl;
                return -1;
              }

              auto file = crailStore->Create<CrailFile>(const_cast<std::string&>(object_key), 0, 0, 1).get();
              if (!file.valid()) {
                cout << "create node failed" << endl;
                return -1;
              }

              unique_ptr<CrailOutputstream> outputstream = file.outputstream();

              shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(kBufferSize);
              while (size_t len = fread(buf->get_bytes(), 1, buf->remaining(), fp)) {
                buf->set_position(buf->position() + len);
                if (buf->remaining() > 0) {
                  continue;
                }

                buf->Flip();
                while (buf->remaining() > 0) {
                  if (outputstream->Write(buf).get() < 0) {
                    return -1;
                  }
                }
                buf->Clear();
              }

              if (buf->position() > 0) {
                buf->Flip();
                while (buf->remaining() > 0) {
                  if (outputstream->Write(buf).get() < 0) {
                    return -1;
                  }
                }
              }
              fclose(fp);
              outputstream->Close().get();

              success_callback( upload_requests[ file_id ] );
              expected_responses++;
            }
          }
        }, thread_index
      );
    }
  }

  for ( auto & thread : threads ) {
    thread.join();
  }

}

void CrailClient::download_files(const std::vector<storage::GetRequest> & download_requests,
                       const std::function<void( const storage::GetRequest & )> & success_callback) {
  const size_t thread_count = 1;
  const size_t batch_size = config_.max_batch_size;

  vector<thread> threads;
  for ( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    if ( thread_index < download_requests.size() ) {
      threads.emplace_back(
        [&] ( const size_t index )
        {
          // try to connect to Crail server
          std::shared_ptr<CrailStore> crailStore;
          crailStore.reset(new CrailStore(config_.namenode_address, config_.port));

          std::cout << "\n[INFO] [download_files] thread index [" << thread_index << "] " << "begin connect to crail";
          crailStore->Initialize();
          std::cout << "\n[INFO] [download_files] thread index [" << thread_index << "] " << "connect to crail server end";
          // we can't check the connect result
          
          for ( size_t first_file_idx = index;
                first_file_idx < download_requests.size();
                first_file_idx += thread_count * batch_size ) {

            string str_data="";
            size_t expected_responses = 0;

            for ( size_t file_id = first_file_idx;
                  file_id < min( download_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & filename = download_requests.at( file_id ).filename.string();
              const string & object_key = download_requests.at( file_id ).object_key;

              CrailFile file = crailStore->Lookup<CrailFile>(const_cast<std::string&>(object_key)).get();
              if (!file.valid()) {
                cout << "lookup node failed" << endl;
                return -1;
              }

              cout << "\n[NOTICE] [download_files] filename: " << filename;

              FILE *fp = fopen(filename.c_str(), "w");
              if (!fp) {
                cout << "could not open local file " << filename.c_str() << endl;
                return -1;
              }

              unique_ptr<CrailInputstream> inputstream = file.inputstream();

              shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(kBufferSize);
              while (inputstream->Read(buf).get() > 0) {
                buf->Flip();
                while (buf->remaining()) {
                  if (size_t len = fwrite(buf->get_bytes(), 1, buf->remaining(), fp)) {
                    buf->set_position(buf->position() + len);
                  } else {
                    break;
                  }
                }
                buf->Clear();
              }
              fclose(fp);
              inputstream->Close();

              success_callback( download_requests[ file_id ] );
              expected_responses++;
            }
          }
        }, thread_index
      );
    }
  }

  for ( auto & thread : threads ) {
    thread.join();
  }
}