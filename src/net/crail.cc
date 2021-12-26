/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

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

#include <crail/client/api/crail_client.h>

using namespace std;
using namespace logger;

// callback is useless at all
void Crail::upload_files(const vector<storage::PutRequest> &upload_requests, const function<void( const storage::PutRequest & )> &success_callback) {
    log_debug("method upload_files begin");

    log_debug("size of upload_requests -> [%d]", upload_requests.size());

    log_debug("begin construct CrailClient obj");
    CrailClient client(this->_config.ip, this->_config.port);
    log_debug("end construct CrailClient obj");

    log_debug("begin connect to crail server");
    if (client.Connect() < 0) {
        throw runtime_error( "error connecting to crail server" );
    }
    log_debug("end connect to crail server");

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
            log_error("failed to set key [%s] to crail", object_key.c_str());
            throw runtime_error("failed to set key");
        }
        log_debug("end set key [%s]", object_key.c_str());

        success_callback(upload_requests[i]);

        log_debug("end process upload_requests[%d]", i);
    }

    log_debug("method upload_files end");
}

// callback is useless at all
void Crail::download_files(const std::vector<storage::GetRequest> &download_requests, const std::function<void( const storage::GetRequest & )> &success_callback) {
    log_debug("method download_files begin");

    log_debug("size of download_requests -> [%d]", download_requests.size());

    log_debug("begin construct CrailClient obj");
    CrailClient client(this->_config.ip, this->_config.port);
    log_debug("end construct CrailClient obj");

    log_debug("begin connect to crail server");
    if (client.Connect() < 0) {
        log_error("error connecting to crail server");
        throw runtime_error( "error connecting to crail server" );
    }
    log_debug("end connect to crail server");

    for (size_t i = 0; i < download_requests.size(); i++) {
        log_debug("begin process download_requests[%d]", i);

        const string & filename = download_requests.at(i).filename.string();
        const string & object_key = download_requests.at(i).object_key;

        log_debug("filename -> [%s]", filename.c_str());
        log_debug("object_key -> [%s]",  object_key.c_str());

        string str_data;

        if (client.Get(object_key, str_data) < 0) {
            log_error("failed to get key [%s] to crail", object_key.c_str());
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