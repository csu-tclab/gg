/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "net/crail.hh"

#include <thread>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <deque>

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
          // create a CrailStore object
          std::shared_ptr<CrailStore> crailStore;
          crailStore.reset(new CrailStore(config_.namenode_address, config_.port));

          // connect to crail server
          printf("\n");
          printf("->[INFO] [upload_files] thread index [%d] begin connect to crail\n", thread_index);
          int conResult = crailStore->Initialize();
          if (conResult < 0) {
            fprintf(stderr, "FAILED to connect to crail server\n");
            throw runtime_error( "error connecting to crail server\n");
          }
          printf("->[INFO] [upload_files] thread index [%d] connected to crail server end\n", thread_index);

          for ( size_t first_file_idx = index;
                first_file_idx < upload_requests.size();
                first_file_idx += thread_count * batch_size ) {
            
            size_t expected_responses = 0;
            std::deque<bool> upload_results;

            for ( size_t file_id = first_file_idx;
                  file_id < min( upload_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & filename = upload_requests.at( file_id ).filename.string();
              string object_key = "/" + upload_requests.at( file_id ).object_key;
              string contents = "";
              bool upload_result = true;

              // check if key dumplicate, if dumplicated then we remove it before PUT
              auto dupCheck = crailStore->Lookup<CrailFile>(object_key).get();
              if (dupCheck.valid()) {
                fprintf(stdout, "[WARN] found dumplicated key, will remove!\n");

                // remove existed key
                auto result = crailStore->Remove(object_key, true);
                if (result != 0) {
                  fprintf(stderr, "[ERROR] remove existed key FAILED!\n");
                  upload_result = false;

                  upload_results.push_back(upload_result);
                  expected_responses++;
                  continue;
                }
              }

              // read file from local disk to c++ string
              FileDescriptor file { CheckSystemCall( "open " + filename, open( filename.c_str(), O_RDONLY ) ) };
              while ( not file.eof() ) { contents.append( file.read() ); }
              file.close();

              // upload all files to crail data server
              printf("[INFO] [upload_files] --> HOST: [%s] filename: [%s] object_key: [%s] BEGIN\n",this->hostname, filename.c_str(), object_key.c_str());
              auto crailFile = crailStore->Create<CrailFile>(const_cast<std::string&>(object_key), 0, 0, 1).get();
              if (!crailFile.valid()) {
                fprintf(stderr, "[ERROR] create crail node FAILED!\n");
                upload_result = false;

                upload_results.push_back(upload_result);
                expected_responses++;
                continue;
              }

              unique_ptr<CrailOutputstream> outputstream = crailFile.outputstream();
              shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(contents.length() + 1);
              buf->PutBytes(const_cast<char *>(contents.c_str()), contents.length());
              buf->Flip();
              while(buf->remaining() > 0) {
                if (outputstream->Write(buf).get() < 0) {
                  fprintf(stderr, "write buf to crail server FAILED!");
                  upload_result = false;
                  
                  upload_results.push_back(upload_result);
                  expected_responses++;
                  continue;
                }
              }

              outputstream->Close().get();
              upload_results.push_back(upload_result);
              printf("[INFO] [upload_files] --> HOST: [%s] filename: [%s] object_key: [%s] END\n",this->hostname, filename.c_str(), object_key.c_str());
              expected_responses++;
            }

            size_t response_count = 0;
            while (!upload_results.empty()) {
              bool result = upload_results.front();

              if (result != true) {
                fprintf(stderr, "there was an ERROR during upload\n");
                throw runtime_error("upload failure in CrailClient::upload_files()\n");
              }

              const size_t response_index = first_file_idx + response_count * thread_count;
              success_callback( upload_requests[ response_index ] );

              response_count++;
              upload_results.pop_front();
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
          int conResult = crailStore->Initialize();
          if (conResult < 0) {
            fprintf(stderr, "FAILED to connect to crail server");
            throw runtime_error( "error connecting to crail server\n");
          }
          printf("<-[INFO] [download_files] thread index [%d] connected to crail end\n", thread_index);
          
          for ( size_t first_file_idx = index;
                first_file_idx < download_requests.size();
                first_file_idx += thread_count * batch_size ) {
            
            size_t expected_responses = 0;
            std::deque<bool> download_results;
            std::deque<std::string> download_contents;

            for ( size_t file_id = first_file_idx;
                  file_id < min( download_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & object_key = "/" + download_requests.at( file_id ).object_key;
              bool download_result = true;
              string str_data = "";

              // lookup and download files
              printf("[INFO] [download_files] <-- HOST: [%s], object_key: [%s] END\n",this->hostname, object_key.c_str());
              CrailFile file = crailStore->Lookup<CrailFile>(const_cast<std::string&>(object_key)).get();
              if (!file.valid()) {
                fprintf(stderr, "[ERROR] lookup node FAILED\n");
                download_result = false;

                download_results.push_back(download_result);
                expected_responses++;
                continue;
              }

              // save file stream to string
              unique_ptr<CrailInputstream> inputstream = file.inputstream();
              shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(kBufferSize);
              while (inputstream->Read(buf).get() > 0) {
                buf->Flip();
                str_data.append(reinterpret_cast<const char*>(buf->get_bytes()), buf->remaining());
                buf->Clear();
              }
              inputstream->Close();
              printf("[INFO] [download_files] <-- HOST: [%s], object_key: [%s] END\n",this->hostname, object_key.c_str());

              // save file string to deque
              download_contents.push_back(str_data);

              download_results.push_back(download_result);
              expected_responses++; 
            }

            size_t response_count = 0;
            while (response_count != expected_responses) {
              bool result = download_results.front();

              if (result != true) {
                fprintf(stderr, "there was an ERROR during download\n");
                throw runtime_error("upload failure in CrailClient::download_files()\n");
              } else {
                std::string str_data = download_contents.front();
                const size_t response_index = first_file_idx + response_count * thread_count;
                const string & filename = download_requests.at( response_index ).filename.string();

                printf("[INFO] create file: [%s] BEGIN\n", filename.c_str());
                roost::atomic_create( str_data, filename,
                                    download_requests[ response_index ].mode.initialized(),
                                    download_requests[ response_index ].mode.get_or( 0 ) );
                printf("[INFO] create file: [%s] END\n", filename.c_str());

                success_callback( download_requests[ response_index ] );
                response_count++;

                download_contents.pop_front();
                download_results.pop_front();
              }
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