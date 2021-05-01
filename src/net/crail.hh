#ifndef NET_CRAIL_HH
#define NET_CRAIL_HH

#include <vector>
#include <string>
#include <functional>
#include <thread>

#include "third_party/crailnative/crail/client/crail_file.h"

#include "net/requests.hh"

struct CrailClientConfig
{
  std::string ip { "0.0.0.0" };
  uint16_t port { 6379 };

  std::string username {};
  std::string password {};

  size_t max_threads { 32 };
  size_t max_batch_size { 32 };
};

class Crail
{
private:
  CrailClientConfig _config;

public:
  Crail( const CrailClientConfig & config )
    : _config( config )
  {}

  void upload_files( const std::vector<storage::PutRequest> & upload_requests,
                     const std::function<void( const storage::PutRequest & )> & success_callback
                       = []( const storage::PutRequest & ){} );

  void download_files( const std::vector<storage::GetRequest> & download_requests,
                       const std::function<void( const storage::GetRequest & )> & success_callback
                         = []( const storage::GetRequest & ){} );
};

#endif