#include "../include/gladiapp/gladiapp_rest.hpp"
#include "../include/gladiapp/impl/gladia_rest_client_boost_impl.hpp"
#include "../include/gladiapp/gladiapp_rest.hpp"

gladiapp::v2::GladiaRestClient::GladiaRestClient(const std::string &apiKey)
{
    _restClientImpl = std::make_unique<GladiaRestClientImpl>(apiKey);
}

gladiapp::v2::GladiaRestClient::~GladiaRestClient()
{
}

response::UploadResponse gladiapp::v2::GladiaRestClient::upload(const std::string &filePath) const
{
    return _restClientImpl->upload(filePath);
}

response::TranscriptionJobResponse gladiapp::v2::GladiaRestClient::preRecorded(const request::TranscriptionRequest &transcriptionRequest,
                                                                               response::TranscriptionError *transcriptionError) const
{
    return _restClientImpl->preRecorded(transcriptionRequest, transcriptionError);
}

response::TranscriptionResult gladiapp::v2::GladiaRestClient::getResult(const std::string &id,
                                                                        response::TranscriptionError *transcriptionError) const
{
    return _restClientImpl->getResult(id, transcriptionError);
}

response::TranscriptionListResults gladiapp::v2::GladiaRestClient::getResults(const request::ListResultsQuery &query,
                                                                         response::TranscriptionError *transcriptionError) const
{
    return _restClientImpl->getResults(query, transcriptionError);
}