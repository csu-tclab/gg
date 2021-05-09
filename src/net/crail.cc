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

// TODO: finish this upload files interface
void CrailClient::upload_files( const std::vector<storage::PutRequest> & upload_requests,
                     const std::function<void( const storage::PutRequest & )> & success_callback) {
  const size_t thread_count = config_.max_threads;
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

          std::cout << "[INFO] thread index[" << thread_index << "]" << "begin connect to crail" << std::endl;
          crailStore->Initialize();
          std::cout << "[INFO] thread index[" << thread_index << "]" << "connect to crail server end" << std::endl;
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

              string contents;
              FileDescriptor file { CheckSystemCall( "open " + filename, open( filename.c_str(), O_RDONLY ) ) };
              while ( not file.eof() ) { contents.append( file.read() ); }
              file.close();

              //TODO: Call Crailnative library to upload file.
              //{

              //}
              auto crailFile = crailStore->Create<CrailFile>(const_cast<std::string&>(object_key), 0, 0, true).get();
              if ( !crailFile.valid() ) {
                throw runtime_error( "failed to create crailFile" );
              }

              unique_ptr<CrailOutputstream> outputstream = crailFile.outputstream();
              std::shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(contents.length());

              buf->PutBytes(contents.c_str(), contents.length());
              buf->Flip();
              while (buf->remaining() > 0) {
                if (outputstream->Write(buf).get() < 0) {
                  throw runtime_error( "failed to write to crailFile outputStream" );
                }
              }
              outputstream->Close().get();

              expected_responses++;

            }

            size_t response_count = 0;
            //TODO: Deal with responses from Crail
            while ( response_count != expected_responses ) {
              /* drain responses */

              // TODO: Get Response from Crail and check it status.

              const size_t response_index = first_file_idx + response_count * thread_count;
              success_callback( upload_requests[ response_index ] );

              response_count++;
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

// TODO: finish this download files interface
void CrailClient::download_files(const std::vector<storage::GetRequest> & download_requests,
                       const std::function<void( const storage::GetRequest & )> & success_callback) {
  const size_t thread_count = config_.max_threads;
  const size_t batch_size = config_.max_batch_size;

  vector<thread> threads;
  for ( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    if ( thread_index < download_requests.size() ) {
      threads.emplace_back(
        [&] ( const size_t index )
        {
          //TODO: Try to connect Crail
          //{

          //}

          for ( size_t first_file_idx = index;
                first_file_idx < download_requests.size();
                first_file_idx += thread_count * batch_size ) {

            size_t expected_responses = 0;

            for ( size_t file_id = first_file_idx;
                  file_id < min( download_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & object_key = download_requests.at( file_id ).object_key;

              //TODO: Call Crailnative library to download file.
              //{

              //}

              expected_responses++;

            }

            size_t response_count = 0;
            //TODO: Deal with responses from Crail
            while ( response_count != expected_responses ) {
              /* drain responses */

              //TODO: Get Response from Crail and check it status.

              const size_t response_index = first_file_idx + response_count * thread_count;
              const string & filename = download_requests.at( response_index ).filename.string();
              //TODO: Write file to filesystem.
              
              //roost::atomic_create( str_data, filename,
              //                      download_requests[ response_index ].mode.initialized(),
              //                      download_requests[ response_index ].mode.get_or( 0 ) );

              success_callback( download_requests[ response_index ] );

              response_count++;
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