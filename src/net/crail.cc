#include "net/crail.hh"

// TODO: finish this upload files interface
void Crail::upload_files( const std::vector<storage::PutRequest> & upload_requests,
                     const std::function<void( const storage::PutRequest & )> & success_callback) {

}

// TODO: finish this download files interface
void Crail::download_files(const std::vector<storage::GetRequest> & download_requests,
                       const std::function<void( const storage::GetRequest & )> & success_callback) {

}