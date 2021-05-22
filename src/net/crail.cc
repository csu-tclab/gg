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

          printf("\n");
          printf("->[INFO] [upload_files] thread index [%d] begin connect to crail\n", thread_index);
          crailStore->Initialize();
          printf("->[INFO] [upload_files] thread index [%d] connect to crail server end\n", thread_index);
          // we can't check the connect result

          for ( size_t first_file_idx = index;
                first_file_idx < upload_requests.size();
                first_file_idx += thread_count * batch_size ) {

            for ( size_t file_id = first_file_idx;
                  file_id < min( upload_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & filename = upload_requests.at( file_id ).filename.string();
              string object_key = "/" + upload_requests.at( file_id ).object_key;

              FILE *fp = fopen(filename.c_str(), "r");
              if (!fp) {
                fprintf(stderr, "[ERROR] could not open local file: [%s]\n", filename.c_str());
                return -1;
              }

              // check if dumplicated
              auto existCheck = crailStore->Lookup<CrailFile>(object_key).get();
              if (existCheck.valid()) {
                fprintf(stderr, "[NOTICE] found dumplicated key, will remove!\n");

                // remove existed key
                auto result = crailStore->Remove(object_key, true);
                if (result != 0) {
                  fprintf(stderr, "[ERROR] remove existed key FAILED!\n");
                  continue;
                }
              }

              auto file = crailStore->Create<CrailFile>(const_cast<std::string&>(object_key), 0, 0, 1).get();
              if (!file.valid()) {
                fprintf(stderr, "[ERROR] create node failed\n");
                return -1;
              }

              printf("->[NOTICE] [upload_files] host: [%s] filename: [%s] object_key: [%s]\n",this->hostname, filename.c_str(), object_key.c_str());
              
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

          printf("<-[INFO] [download_files] thread index [%d] begin connect to crail\n", thread_index);
          crailStore->Initialize();
          printf("<-[INFO] [download_files] thread index [%d] connect to crail end\n", thread_index);
          // we can't check the connect result
          
          for ( size_t first_file_idx = index;
                first_file_idx < download_requests.size();
                first_file_idx += thread_count * batch_size ) {

            for ( size_t file_id = first_file_idx;
                  file_id < min( download_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & filename = download_requests.at( file_id ).filename.string();
              const string & object_key = "/" + download_requests.at( file_id ).object_key;

              CrailFile file = crailStore->Lookup<CrailFile>(const_cast<std::string&>(object_key)).get();
              if (!file.valid()) {
                fprintf(stderr, "[ERROR] lookup node failed\n");
                return -1;
              }

              printf("<-[NOTICE] [download_files] host: [%s] filename: [%s], object_key: [%s]\n",this->hostname, filename.c_str(), object_key.c_str());

              unique_ptr<CrailInputstream> inputstream = file.inputstream();
              string str_data = "";

              shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(kBufferSize);

              while (inputstream->Read(buf).get() > 0) {
                buf->Flip();

                str_data.append(reinterpret_cast<const char*>(buf->get_bytes()), buf->remaining());
                
                buf->Clear();
              }
              
              inputstream->Close();

              roost::atomic_create( str_data, filename,
                                    download_requests[ file_id ].mode.initialized(),
                                    download_requests[ file_id ].mode.get_or( 0 ) );

              success_callback( download_requests[ file_id ] );
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