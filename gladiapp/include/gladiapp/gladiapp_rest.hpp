#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <memory>

namespace gladiapp
{
    namespace v2
    {
        namespace response
        {
            /**
             * Represents metadata about an audio file.
             * Contains information such as the file's ID, filename, and other properties.
             */
            struct AudioMetadata
            {
                std::string id;
                std::string filename;
                std::string extension;
                std::size_t size;
                double audio_duration;
                int number_of_channels;

                static AudioMetadata fromJson(const std::string &jsonString)
                {
                    AudioMetadata metadata;
                    auto json = nlohmann::json::parse(jsonString);

                    metadata.id = json.value("id", "");
                    metadata.filename = json.value("filename", "");
                    metadata.extension = json.value("extension", "");
                    metadata.size = json.value("size", 0);
                    metadata.audio_duration = json.value("audio_duration", 0.0);
                    metadata.number_of_channels = json.value("number_of_channels", 0);

                    return metadata;
                }

                std::string toString() const
                {
                    nlohmann::json j;
                    j["id"] = id;
                    j["filename"] = filename;
                    j["extension"] = extension;
                    j["size"] = size;
                    j["audio_duration"] = audio_duration;
                    j["number_of_channels"] = number_of_channels;
                    return j.dump();
                }
            };

            /**
             * Represents the response for an audio request.
             * Contains the audio file's URL and its metadata.
             */
            struct UploadResponse
            {
                std::string audio_url;
                AudioMetadata audio_metadata;

                static UploadResponse fromJson(const std::string &jsonString)
                {
                    UploadResponse response;
                    auto json = nlohmann::json::parse(jsonString);

                    response.audio_url = json.value("audio_url", "");

                    if (json.contains("audio_metadata"))
                    {
                        response.audio_metadata = AudioMetadata::fromJson(json["audio_metadata"].dump());
                    }

                    return response;
                }

                std::string toString() const
                {
                    nlohmann::json j;
                    j["audio_url"] = audio_url;
                    j["audio_metadata"] = nlohmann::json::parse(audio_metadata.toString());
                    return j.dump();
                }
            };

            /**
             * Represents the response for a transcription job request.
             * Contains the transcription's ID and the URL to the result.
             */
            struct TranscriptionJobResponse
            {
                std::string id;
                std::string result_url;

                static TranscriptionJobResponse fromJson(const std::string &jsonString)
                {
                    TranscriptionJobResponse response;
                    auto json = nlohmann::json::parse(jsonString);
                    response.id = json.value("id", "");
                    response.result_url = json.value("result_url", "");
                    return response;
                }

                std::string toString() const
                {
                    nlohmann::json j;
                    j["id"] = id;
                    j["result_url"] = result_url;
                    return j.dump();
                }
            };

            /**
             * Represents an error that occurs during transcription.
             * Contains information about the error's timestamp, path, and message.
             * Additionally, it includes the request ID, status code, and any validation errors.
             */
            struct TranscriptionError
            {
                std::string timestamp;
                std::string path;
                std::string request_id;
                int status_code;
                std::string message;
                std::vector<std::string> validation_errors;

                static TranscriptionError fromJson(const std::string &jsonString)
                {
                    TranscriptionError error;
                    auto json = nlohmann::json::parse(jsonString);

                    error.timestamp = json.value("timestamp", "");
                    error.path = json.value("path", "");
                    error.request_id = json.value("request_id", "");
                    error.status_code = json.value("statusCode", 0);
                    error.message = json.value("message", "");

                    if (json.contains("validation_errors") && json["validation_errors"].is_array())
                    {
                        for (const auto &validation_error : json["validation_errors"])
                        {
                            if (validation_error.is_string())
                            {
                                error.validation_errors.push_back(validation_error.get<std::string>());
                            }
                        }
                    }

                    return error;
                }

                std::string toString() const
                {
                    nlohmann::json j;
                    j["timestamp"] = timestamp;
                    j["path"] = path;
                    j["request_id"] = request_id;
                    j["statusCode"] = status_code;
                    j["message"] = message;
                    j["validation_errors"] = validation_errors;
                    return j.dump();
                }
            };
        }

        namespace request
        {
            /**
             * Represents information about an audio file.
             * Contains the audio file's URL and its metadata.
             */
            struct DiarizationConfig
            {
                int number_of_speakers;
                int min_speakers;
                int max_speakers;
                std::string toJson() const
                {
                    // Convert the DiarizationConfig object to a JSON string
                    // You can use a JSON library like nlohmann/json for this
                    nlohmann::json j;
                    j["number_of_speakers"] = number_of_speakers;
                    j["min_speakers"] = min_speakers;
                    j["max_speakers"] = max_speakers;
                    return j.dump();
                }
            };

            /**
             * Represents the configuration for audio translation.
             * Contains the model to be used and the target languages for translation.
             */
            struct TranslationConfig
            {
                std::string model;
                std::vector<std::string> target_languages;
                bool context_adaptation;
                std::string context;
                bool informal;

                std::string toJson() const
                {
                    // Convert the TranslationConfig object to a JSON string
                    // You can use a JSON library like nlohmann/json for this
                    nlohmann::json j;
                    j["model"] = model;
                    j["target_languages"] = target_languages;
                    j["context_adaptation"] = context_adaptation;
                    j["context"] = context;
                    j["informal"] = informal;
                    return j.dump();
                }
            };

            /**
             * Represents the configuration for subtitles.
             * Contains the formats for the generated subtitles.
             */
            struct SubtitlesConfig
            {
                std::vector<std::string> formats;

                std::string toJson() const
                {
                    // Convert the SubtitlesConfig object to a JSON string
                    // You can use a JSON library like nlohmann/json for this
                    nlohmann::json j;
                    j["formats"] = formats;
                    return j.dump();
                }
            };

            /**
             * Represents the configuration for audio requests.
             * Contains the audio file's URL and its processing options.
             */
            struct AudioRequest
            {
                std::string audio_url;
                bool diarization;
                DiarizationConfig diarization_config;
                bool translation;
                TranslationConfig translation_config;
                bool subtitles;
                SubtitlesConfig subtitles_config;
                bool detect_language;
                bool enable_code_switching;

                std::string toJson() const
                {
                    // Convert the AudioRequest object to a JSON string
                    // You can use a JSON library like nlohmann/json for this
                    nlohmann::json j;
                    j["audio_url"] = audio_url;
                    j["diarization"] = diarization;
                    j["diarization_config"] = diarization_config.toJson();
                    j["translation"] = translation;
                    j["translation_config"] = translation_config.toJson();
                    j["subtitles"] = subtitles;
                    j["subtitles_config"] = subtitles_config.toJson();
                    j["detect_language"] = detect_language;
                    j["enable_code_switching"] = enable_code_switching;
                    return j.dump();
                }
            };
        }

        /**
         * forward declaration of the actual implementation
         */
        class GladiaRestClientImpl;

        class GladiaRestClient
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
            response::UploadResponse upload(const std::string &filePath);

            /**
             * Sends a pre-recorded audio url for processing.
             * @param fileUrl The URL of the audio file to process.
             * @return The response from the server.
             */
            response::UploadResponse preRecorded(const std::string &fileUrl);

        private:
            std::unique_ptr<GladiaRestClientImpl> _restClientImpl;
        };
    }
}