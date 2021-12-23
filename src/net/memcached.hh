/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef NET_MEMCACHED_HH
#define NET_MEMCACHED_HH

#include <vector>
#include <string>
#include <functional>

#include <libmemcached/memcached.hpp>

#include "net/requests.hh"

struct MemcachedClientConfig {
    std::string ip {"0.0.0.0"};
    uint16_t port {6379};

    std::string username {};
    std::string password {};

    size_t max_threads { 32 };
    size_t max_batch_size { 32 };
};

class Memcached {
public:
    Memcached(const MemcachedClientConfig & config ) : _config(config) {};

    void upload_files(const std::vector<storage::PutRequest> &upload_requests,
                      const std::function<void( const storage::PutRequest & )> &success_callback = []( const storage::PutRequest & ){});

    void download_files(const std::vector<storage::GetRequest> &download_requests,
                        const std::function<void( const storage::GetRequest & )> &success_callback = []( const storage::GetRequest & ){});

private:

private:
    MemcachedClientConfig _config;  // config
};

class MemcachedClient {
public:
    explicit MemcachedClient(MemcachedClientConfig &config);
    ~MemcachedClient();
    
    // non-copyable
    MemcachedClient(const MemcachedClient& memcachedClient) = delete;
    MemcachedClient& operator=(const MemcachedClient& memcachedClient) = delete;

    int Connect();

    int Set(const std::string key, const std::string &value);
    int Get(const std::string key, std::string &value);
    
private:
    int Disconnect();

private:
    // connect instance
    memcached_st *_mem_connect;

    // addr
    std::string _addr;

    // port
    uint16_t _port;
};

#endif /* NET_MEMCACHED_HH */
