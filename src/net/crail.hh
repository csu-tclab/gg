/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef NET_CRAIL_HH
#define NET_CRAIL_HH

#include <vector>
#include <string>
#include <functional>

#include "net/requests.hh"

struct CrailClientConfig
{
  std::string namenode_address { "0.0.0.0" };
  uint16_t port { 9060 };

  size_t max_threads { 32 };
  size_t max_batch_size { 32 };
};

class CrailClient
{
private:
  CrailClientConfig config_;

public:
  CrailClient( const CrailClientConfig & config ) : config_( config ) {}

  void upload_files( const std::vector<storage::PutRequest> & upload_requests,
                     const std::function<void( const storage::PutRequest & )> & success_callback
                       = []( const storage::PutRequest & ){} );

  void download_files( const std::vector<storage::GetRequest> & download_requests,
                       const std::function<void( const storage::GetRequest & )> & success_callback
                         = []( const storage::GetRequest & ){} );
};

#endif /* CRAIL_HH */