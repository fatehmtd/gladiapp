#pragma once

#include "../gladiapp_rest.hpp"
#include "../gladiapp.hpp"
#include "curl_http_util.hpp"
#include <sstream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

using namespace gladiapp::v2;

namespace gladiapp
{
    namespace v2
    {
        class GladiaRestClientImpl
        {
        public:
            GladiaRestClientImpl(const std::string &apiKey) : _apiKey(apiKey)
            {
            }

            ~GladiaRestClientImpl()
            {
            }

            response::UploadResponse upload(const std::string &filePath, response::TranscriptionError *transcriptionError) const
            {
                try
                {
                    auto httpResponse = curl_util::performUpload(
                        curl_util::buildUrl(gladiapp::v2::common::UPLOAD_ENDPOINT), _apiKey, filePath);

                    if (httpResponse.statusCode != 200)
                    {
                        if (transcriptionError != nullptr)
                        {
                            *transcriptionError = response::TranscriptionError::fromJson(httpResponse.body);
                            return response::UploadResponse();
                        }
                    }
                    return response::UploadResponse::fromJson(httpResponse.body);
                }
                catch (const std::exception &e)
                {
                    spdlog::error("Error occurred: {}", e.what());
                }
                return response::UploadResponse();
            }

            response::TranscriptionJobResponse preRecorded(const request::TranscriptionRequest &transcriptionRequest,
                                                           response::TranscriptionError *transcriptionError = nullptr) const
            {
                try
                {
                    auto httpResponse = curl_util::performRequest(
                        curl_util::buildUrl(gladiapp::v2::common::PRERECORDED_ENDPOINT), "POST", _apiKey,
                        transcriptionRequest.toJson().dump(), "application/json");

                    spdlog::info("!!! Transcription response: {}, http code: {}", httpResponse.body, httpResponse.statusCode);

                    if (httpResponse.statusCode != 201)
                    {
                        std::ostringstream oss;
                        oss << "Transcription failed, error code: " << httpResponse.statusCode
                            << ", message: " << httpResponse.body;
                        if (transcriptionError != nullptr)
                        {
                            *transcriptionError = response::TranscriptionError::fromJson(httpResponse.body);
                            return response::TranscriptionJobResponse();
                        }
                    }
                    return response::TranscriptionJobResponse::fromJson(httpResponse.body);
                }
                catch (const std::exception &e)
                {
                    spdlog::error("Error occurred: {}", e.what());
                }

                return response::TranscriptionJobResponse();
            }

            response::TranscriptionResult getResult(const std::string &id,
                                                    response::TranscriptionError *transcriptionError) const
            {
                try
                {
                    auto httpResponse = curl_util::performRequest(
                        curl_util::buildUrl(std::string(gladiapp::v2::common::PRERECORDED_ENDPOINT) + "/" + id),
                        "GET", _apiKey);

                    if (httpResponse.statusCode != 200)
                    {
                        std::ostringstream oss;
                        oss << "Failed to get transcription result, error code: " << httpResponse.statusCode
                            << ", message: " << httpResponse.body;
                        if (transcriptionError != nullptr)
                        {
                            *transcriptionError = response::TranscriptionError::fromJson(httpResponse.body);
                            return response::TranscriptionResult();
                        }
                    }
                    else
                    {
                        spdlog::info("Successfully retrieved transcription result: {}", httpResponse.body);
                        return response::TranscriptionResult::fromJson(httpResponse.body);
                    }
                }
                catch (const std::exception &e)
                {
                    spdlog::error("Error occurred: {}", e.what());
                    spdlog::throw_spdlog_ex(e.what());
                }
                return response::TranscriptionResult();
            }

            response::TranscriptionListResults getResults(const request::ListResultsQuery &query,
                                                          response::TranscriptionError *transcriptionError) const
            {
                try
                {
                    std::ostringstream stringStream;
                    stringStream << gladiapp::v2::common::PRERECORDED_ENDPOINT << "?"
                                 << "offset=" << query.offset
                                 << "&limit=" << query.limit;
                    if (!query.date.empty())
                        stringStream << "&date=" << curl_util::urlEncode(query.date);
                    if (!query.before_date.empty())
                        stringStream << "&before_date=" << curl_util::urlEncode(query.before_date);
                    if (!query.after_date.empty())
                        stringStream << "&after_date=" << curl_util::urlEncode(query.after_date);
                    if (!query.status.empty())
                    {
                        stringStream << "&status=";

                        for (size_t i = 0; i < query.status.size(); ++i)
                        {
                            if (i > 0)
                            {
                                stringStream << ",";
                            }
                            switch (query.status[i])
                            {
                            case request::ListResultsQuery::Status::DONE:
                                stringStream << "done";
                                break;
                            case request::ListResultsQuery::Status::ERROR:
                                stringStream << "error";
                                break;
                            case request::ListResultsQuery::Status::PROCESSING:
                                stringStream << "processing";
                                break;
                            case request::ListResultsQuery::Status::QUEUED:
                                stringStream << "queued";
                                break;
                            }
                        }
                    }

                    auto httpResponse = curl_util::performRequest(
                        curl_util::buildUrl(stringStream.str()), "GET", _apiKey);

                    if (httpResponse.statusCode != 200)
                    {
                        std::ostringstream oss;
                        oss << "Failed to get transcription result, error code: " << httpResponse.statusCode
                            << ", message: " << httpResponse.body;
                        if (transcriptionError != nullptr)
                        {
                            *transcriptionError = response::TranscriptionError::fromJson(httpResponse.body);
                            return response::TranscriptionListResults();
                        }
                    }
                    else
                    {
                        spdlog::info("Successfully retrieved transcription result: {}", httpResponse.body);
                        return response::TranscriptionListResults::fromJson(httpResponse.body);
                    }
                }
                catch (const std::exception &e)
                {
                    spdlog::error("Error occurred: {}", e.what());
                    spdlog::throw_spdlog_ex(e.what());
                }

                return response::TranscriptionListResults();
            }

            void deleteResult(const std::string &id, response::TranscriptionError *transcriptionError) const
            {
                try
                {
                    auto httpResponse = curl_util::performRequest(
                        curl_util::buildUrl(std::string(gladiapp::v2::common::PRERECORDED_ENDPOINT) + "/" + id),
                        "DELETE", _apiKey);

                    if (httpResponse.statusCode != 202)
                    {
                        std::ostringstream oss;
                        oss << "Failed to delete transcription result, error code: " << httpResponse.statusCode
                            << ", message: " << httpResponse.body;
                        spdlog::error(oss.str());
                        if (transcriptionError != nullptr)
                        {
                            *transcriptionError = response::TranscriptionError::fromJson(httpResponse.body);
                        }
                    }
                    else
                    {
                        spdlog::info("Successfully deleted result: {}", id);
                    }
                }
                catch (const std::exception &e)
                {
                    spdlog::error("Error occurred: {}", e.what());
                    spdlog::throw_spdlog_ex(e.what());
                }
            }

        private:
            std::string _apiKey;
        };
    }
}
