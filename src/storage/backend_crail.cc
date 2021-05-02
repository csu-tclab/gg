#include "storage/backend_crail.hh"

using namespace std;
using namespace storage;

void CrailStorageBackend::get( const std::vector<storage::GetRequest> & requests,
            const GetCallback & success_callback ) {
    this->client_.download_files(requests, success_callback);
}

void CrailStorageBackend::put( const std::vector<storage::PutRequest> & requests,
            const PutCallback & success_callback ) {
    this->client_.upload_files(requests, success_callback);
}