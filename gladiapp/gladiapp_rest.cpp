#include "./include/gladiapp/gladiapp_rest.hpp"
#include "./include/gladiapp/impl/gladia_rest_client_boost_impl.hpp"

gladiapp::v2::GladiaRestClient::GladiaRestClient(const std::string &apiKey)
{
    _restClientImpl = std::make_unique<GladiaRestClientImpl>(apiKey);
}

gladiapp::v2::GladiaRestClient::~GladiaRestClient()
{
}

response::UploadResponse gladiapp::v2::GladiaRestClient::upload(const std::string &filePath)
{
    return _restClientImpl->upload(filePath);
}

response::UploadResponse gladiapp::v2::GladiaRestClient::preRecorded(const std::string &fileUrl)
{
    return _restClientImpl->preRecorded(fileUrl);
}
