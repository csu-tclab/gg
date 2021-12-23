/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "net/memcached.hh"

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

void Memcached::upload_files(const vector<storage::PutRequest> &upload_requests, const function<void( const storage::PutRequest & )> &success_callback) {

}

void Memcached::download_files(const std::vector<storage::GetRequest> &download_requests, const std::function<void( const storage::GetRequest & )> &success_callback) {
    
}

MemcachedClient::MemcachedClient(MemcachedClientConfig &config)  : _mem_connect(nullptr), _addr(""), _port(0) {
    // network info
    this->_addr = config.ip;
    this->_port = config.port;

    // no auth for now
}

MemcachedClient::~MemcachedClient() {
    try {
        this->Disconnect();
    } catch(...) {
        // ignore exceptions
    }
}

int MemcachedClient::Connect() {
    int ret = 0;

    // check if connected at first
    if (this->_mem_connect != nullptr) {
        cout << "already connected! operation abort" << endl;
        return 0;
    }

    // retcode for memcached
    memcached_return mem_ret;
    memcached_server_st *mem_server = nullptr;

    // invoke connect
    mem_server = memcached_server_list_append(mem_server, this->_addr.c_str(), this->_port, &mem_ret);
    mem_ret = memcached_server_push(this->_mem_connect, mem_server); 

    // check result
    if (mem_ret != MEMCACHED_SUCCESS) {
        cout << "memcached_server_push failed! rc -> " << mem_ret << endl;
        ret = -1;
    }

    // free server_list
    memcached_server_list_free(mem_server);
    return ret;
}

int MemcachedClient::Disconnect() {
    // invoke disconnect
    if (this->_mem_connect != nullptr) {
        memcached_free(this->_mem_connect);
        this->_mem_connect = nullptr;
    }

    return 0;
}

int MemcachedClient::Set(const std::string key, const std::string &value) {
    int ret = 0;
    uint32_t flags = 0;
    uint32_t expireation = 0;   // won't expire forever
    memcached_return mem_ret;

    assert(this->_mem_connect != nullptr);

    mem_ret = memcached_set(this->_mem_connect, key.c_str(), key.length(), value.c_str(), value.length(), expireation, flags);

    if (mem_ret != MEMCACHED_SUCCESS)
    {
        cout << "set key -> " << key << " failed" << endl;
        ret = -1;
    }

    return ret;
}

int MemcachedClient::Get(const std::string key, std::string &value) {
    uint32_t flags = 0;
    size_t value_length = 0;
    int ret = 0;
    memcached_return mem_ret;

    assert(this->_mem_connect != nullptr);

    char* val = memcached_get(this->_mem_connect, key.c_str(), key.length(), &value_length, &flags, &mem_ret);

    if(mem_ret == MEMCACHED_SUCCESS)
    {  
        value = std::string(val, value_length);
        ret = 0;
    } else {
        ret = -1;
    }

    return ret;
}