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

            GladiaRestClient(const std::string &apiKey);
            ~GladiaRestClient();

            /**
             * Uploads an audio file for processing.
             * @param filePath The path to the audio file to upload.
             * @return The response from the server.
             */
            response::UploadResponse upload(const std::string &filePath) const;

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