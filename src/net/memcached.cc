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
using namespace logger;

// callback is useless at all
void Memcached::upload_files(const vector<storage::PutRequest> &upload_requests, const function<void( const storage::PutRequest & )> &success_callback) {
    log_debug("method upload_files begin");

    log_debug("size of upload_requests -> [%d]", upload_requests.size());

    log_debug("begin construct MemcachedClient obj");
    MemcachedClient client(this->_config);
    log_debug("end construct MemcachedClient obj");

    log_debug("begin connect to memcached server");
    if (client.Connect() < 0) {
        throw runtime_error( "error connecting to memcached server" );
    }
    log_debug("end connect to memcached server");

    // upload all files to storage
    for (size_t i = 0; i < upload_requests.size(); i++) {
        log_debug("begin process upload_requests[%d]", i);

        const string & filename = upload_requests.at(i).filename.string();
        const string & object_key = upload_requests.at(i).object_key;

        log_debug("filename -> [%s]", filename.c_str());
        log_debug("object_key -> [%s]",  object_key.c_str());

        string contents;

        FileDescriptor file { CheckSystemCall( "open " + filename, open( filename.c_str(), O_RDONLY ) ) };
        while ( not file.eof() ) { contents.append( file.read() ); }
        file.close();

        log_debug("begin set key [%s]", object_key.c_str());
        if (client.Set(object_key, contents) < 0) {
            log_error("failed to set key [%s] to memcached", object_key.c_str());
            throw runtime_error("failed to set key");
        }
        log_debug("end set key [%s]", object_key.c_str());

        success_callback(upload_requests[i]);

        log_debug("end process upload_requests[%d]", i);
    }

    log_debug("method upload_files end");
}

// callback is useless at all
void Memcached::download_files(const std::vector<storage::GetRequest> &download_requests, const std::function<void( const storage::GetRequest & )> &success_callback) {
    log_debug("method download_files begin");

    log_debug("size of download_requests -> [%d]", download_requests.size());

    log_debug("begin construct MemcachedClient obj");
    MemcachedClient client(this->_config);
    log_debug("end construct MemcachedClient obj");

    log_debug("begin connect to memcached server");
    if (client.Connect() < 0) {
        log_error("error connecting to memcached server");
        throw runtime_error( "error connecting to memcached server" );
    }
    log_debug("end connect to memcached server");

    for (size_t i = 0; i < download_requests.size(); i++) {
        log_debug("begin process download_requests[%d]", i);

        const string & filename = download_requests.at(i).filename.string();
        const string & object_key = download_requests.at(i).object_key;

        log_debug("filename -> [%s]", filename.c_str());
        log_debug("object_key -> [%s]",  object_key.c_str());

        string str_data;

        if (client.Get(object_key, str_data) < 0) {
            log_error("failed to get key [%s] to memcached", object_key.c_str());
            throw runtime_error("failed to get key");
        }

        log_info("begin create file [%s] for key [%s]", filename.c_str(), object_key.c_str());
        roost::atomic_create(str_data, filename, download_requests[i].mode.initialized(), download_requests[i].mode.get_or( 0 ));
        log_info("end create file [%s] for key [%s]", filename.c_str(), object_key.c_str());

        success_callback( download_requests[i]);

        log_debug("end process download_requests[%d]", i);
    }

    log_debug("method download_files end");
}

MemcachedClient::MemcachedClient(MemcachedClientConfig &config)  : _mem_connect(nullptr), _addr(""), _port(0), _connected(false) {
    // network info
    this->_addr = config.ip;
    this->_port = config.port;

    log_info("ADDR -> [%s], PORT -> [%d]", this->_addr.c_str(), this->_port);
    // no auth for now

    // create memcached_st obj
    this->_mem_connect = memcached_create(NULL);
}

MemcachedClient::~MemcachedClient() {
    try {
        this->Disconnect();
    } catch(...) {
        // ignore exceptions
    }

    memcached_free(this->_mem_connect);
}

int MemcachedClient::Connect() {
    int ret = 0;

    if (this->_connected) {
        log_debug("already connected! abort");
        return 0;
    }

    // retcode for memcached
    memcached_return mem_ret;
    memcached_server_st *mem_server = nullptr;

    log_debug("Connect to [%s]:[%d] begin", this->_addr.c_str(), this->_port);
    // invoke connect
    mem_server = memcached_server_list_append(mem_server, this->_addr.c_str(), this->_port, &mem_ret);
    mem_ret = memcached_server_push(this->_mem_connect, mem_server); 
    log_debug("Connect to [%s]:[%d] end", this->_addr.c_str(), this->_port);
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
        log_debug("Disconnect from [%s]:[%d] begin", this->_addr.c_str(), this->_port);
        memcached_free(this->_mem_connect);
        this->_mem_connect = nullptr;
        this->_connected = false;
        log_debug("Disconnect end");
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
        log_error("set key [%s] failed! ret -> [%d]", key.c_str(), mem_ret);
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
        log_error("get key [%s] failed! ret -> [%d]", key.c_str(), mem_ret);
        ret = -1;
    }

    return ret;
}