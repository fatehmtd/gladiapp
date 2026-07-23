#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <memory>

#include "gladiapp_export.h"
#include "gladiapp_error.hpp"

#include "gladiapp_rest_request.hpp"
#include "gladiapp_rest_response.hpp"

namespace gladiapp
{
    namespace v2
    {
        /**
         * forward declaration of the actual implementation
         */
        class GladiaRestClientImpl;

        class GLADIAPP_EXPORT GladiaRestClient
        {
        public:
            GladiaRestClient() = delete;
            GladiaRestClient(const GladiaRestClient &) = delete;
            GladiaRestClient &operator=(const GladiaRestClient &) = delete;

            /**
             * @param apiKey Gladia API key.
             * @param caFilePath Optional path to a CA bundle (PEM) file for TLS verification.
             *        Leave empty to use curl's platform default trust store; needed on
             *        platforms where the TLS backend has no visibility into the OS trust
             *        store (e.g. Android with mbedTLS).
             */
            GladiaRestClient(const std::string &apiKey, const std::string &caFilePath = {});
            ~GladiaRestClient();

            /**
             * Uploads an audio file for processing.
             * @param filePath The path to the audio file to upload.
             * @return The response from the server.
             */
            response::UploadResponse upload(const std::string &filePath, response::TranscriptionError *transcriptionError) const;

            /**
             * Sends a pre-recorded audio url for processing.
             * @param transcriptionRequest The request containing the audio URL and other parameters.
             * @return The response from the server.
             */
            response::TranscriptionJobResponse preRecorded(const request::TranscriptionRequest &transcriptionRequest,
                                                           response::TranscriptionError *transcriptionError = nullptr) const;

            /**
             * Retrieves the result of a transcription job.
             * @param id The ID of the transcription job.
             * @return The response from the server.
             */
            response::TranscriptionResult getResult(const std::string &id,
                                                    response::TranscriptionError *transcriptionError = nullptr) const;

            /**
             * Retrieves the results of all transcription jobs.
             * @return A vector of responses from the server.
             */
            response::TranscriptionListResults getResults(const request::ListResultsQuery &query,
                                                          response::TranscriptionError *transcriptionError = nullptr) const;

            /**
             * Deletes a transcription job.
             * @param id The ID of the transcription job.
             */
            void deleteResult(const std::string &id,
                              response::TranscriptionError *transcriptionError = nullptr) const;

        private:
            std::unique_ptr<GladiaRestClientImpl> _restClientImpl;
        };
    }
}