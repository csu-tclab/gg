/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend_memcached.hh"

using namespace std;
using namespace storage;

void MemcachedStorageBackend::put( const std::vector<PutRequest> & requests,
                               const PutCallback & success_callback )
{
  client_.upload_files( requests, success_callback );
}

void MemcachedStorageBackend::get( const std::vector<GetRequest> & requests,
                               const GetCallback & success_callback )
{
  client_.download_files( requests, success_callback );
}
