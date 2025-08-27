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
                std::optional<bool> enhanced;
                nlohmann::json toJson() const
                {
                    // Convert the DiarizationConfig object to a JSON string
                    // You can use a JSON library like nlohmann/json for this
                    nlohmann::json j;
                    j["number_of_speakers"] = number_of_speakers;
                    j["min_speakers"] = min_speakers;
                    j["max_speakers"] = max_speakers;
                    if (enhanced.has_value())
                    {
                        j["enhanced"] = enhanced.value() ? "true" : "false";
                    }
                    return j;
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
                std::optional<bool> match_original_utterances;
                std::optional<bool> lipsync = true;
                std::optional<bool> context_adaptation;
                std::optional<std::string> context;
                std::optional<bool> informal = false;

                nlohmann::json toJson() const
                {
                    nlohmann::json j;
                    j["model"] = model;
                    j["target_languages"] = target_languages;
                    if (match_original_utterances.has_value())
                    {
                        j["match_original_utterances"] = match_original_utterances.value() ? "true" : "false";
                    }
                    if (lipsync.has_value())
                    {
                        j["lipsync"] = lipsync.value() ? "true" : "false";
                    }
                    if (context_adaptation.has_value())
                    {
                        j["context_adaptation"] = context_adaptation.value() ? "true" : "false";
                    }
                    if (context.has_value())
                    {
                        j["context"] = context.value();
                    }
                    if (informal.has_value())
                    {
                        j["informal"] = informal.value() ? "true" : "false";
                    }
                    return j;
                }
            };

            /**
             * Represents the configuration for subtitles.
             * Contains the formats for the generated subtitles.
             */
            struct SubtitlesConfig
            {
                enum Format
                {
                    SRT,
                    VTT
                };
                std::vector<Format> formats;

                std::optional<int> maximum_characters_per_row;
                std::optional<int> maximum_rows_per_caption;

                enum Style
                {
                    DEFAULT,
                    COMPLIANCE
                };
                std::optional<Style> style;

                std::string toString() const
                {
                    // Convert the SubtitlesConfig object to a JSON string
                    // You can use a JSON library like nlohmann/json for this
                    nlohmann::json j;
                    j["formats"] = formats;
                    j["maximum_characters_per_row"] = maximum_characters_per_row;
                    j["maximum_rows_per_caption"] = maximum_rows_per_caption;
                    j["style"] = style;
                    return j.dump();
                }
            };

            struct CustomVocabularyConfig
            {
                std::string value;
                std::vector<std::string> pronunciations;
                std::optional<float> intensity;
                std::optional<std::string> language;
            };

            struct CallbackConfig
            {
                std::string url;

                enum class HttpMethod
                {
                    POST,
                    PUT
                };
                /**
                 * HTTP method to use for the callback, only: POST, PUT are supported.
                 */
                HttpMethod method;

                std::string toJson() const
                {
                    nlohmann::json j;
                    j["url"] = url;
                    j["method"] = (method == HttpMethod::POST) ? "POST" : "PUT";
                    return j.dump();
                }
            };

            struct SummarizationConfig
            {
                enum Type
                {
                    GENERAL,
                    BULLET_POINTS,
                    CONCISE
                };
                std::vector<Type> types;

                std::string toJson() const
                {
                    nlohmann::json j;
                    std::vector<std::string> typeStrings;
                    for (const auto &type : types)
                    {
                        switch (type)
                        {
                        case GENERAL:
                            typeStrings.push_back("general");
                            break;
                        case BULLET_POINTS:
                            typeStrings.push_back("bullet_points");
                            break;
                        case CONCISE:
                            typeStrings.push_back("concise");
                            break;
                        }
                    }
                    j["summarization_config"] = typeStrings;
                    return j.dump();
                }
            };

            struct CustomSpellingConfig
            {
                std::unordered_map<std::string, std::vector<std::string>> spelling_dictionary;

                nlohmann::json toJson() const
                {
                    nlohmann::json j;
                    j["spelling_dictionary"] = spelling_dictionary;
                    return j;
                }
            };

            struct StructuredDataExtractionConfig
            {
                std::vector<std::string> classes;

                nlohmann::json toJson() const
                {
                    nlohmann::json j;
                    j["structured_data_extraction_config"] = classes;
                    return j;
                }
            };

            struct AudioToLLMConfig
            {
                std::vector<std::string> prompts;

                nlohmann::json toJson() const
                {
                    nlohmann::json j;
                    j["prompts"] = prompts;
                    return j;
                }
            };

            struct LanguageConfig
            {
                std::vector<std::string> languages;
                bool code_switching = false;

                nlohmann::json toJson() const
                {
                    nlohmann::json j;
                    j["languages"] = languages;
                    j["code_switching"] = code_switching;
                    return j;
                }
            };

            /**
             * Represents the configuration for audio requests.
             * Contains the audio file's URL and its processing options.
             */
            struct AudioRequest
            {
                std::string audio_url;

                bool customVocabulary = false;
                std::vector<CustomVocabularyConfig> custom_vocabulary_config;

                bool callback = false;
                std::optional<CallbackConfig> callback_config;

                bool subtitles = false;
                std::optional<SubtitlesConfig> subtitles_config;

                bool diarization = false;
                std::optional<DiarizationConfig> diarization_config;

                bool translation = false;
                TranslationConfig translation_config;

                bool summarization = false;
                std::optional<SummarizationConfig> summarization_config;

                bool moderation = false;

                bool named_entity_recognition = false;

                bool chapterization = false;

                bool name_consistency = false;

                bool custom_spelling = false;
                std::optional<CustomSpellingConfig> custom_spelling_config;

                bool structured_data_extraction = false;
                std::optional<StructuredDataExtractionConfig> structured_data_extraction_config;

                bool sentiment_analysis = false;

                bool audio_to_llm = false;
                std::optional<AudioToLLMConfig> audio_to_llm_config;

                bool sentences = false;

                bool display_mode = false;

                bool punctuation_enhanced = false;

                std::optional<LanguageConfig> language_config;

                std::string toJson() const
                {
                    nlohmann::json j;

                    j["audio_url"] = audio_url;

                    // Custom vocabulary
                    j["custom_vocabulary"] = customVocabulary;
                    if (customVocabulary && !custom_vocabulary_config.empty())
                    {
                        nlohmann::json vocab_array = nlohmann::json::array();
                        for (const auto &vocab : custom_vocabulary_config)
                        {
                            nlohmann::json vocab_obj;
                            vocab_obj["value"] = vocab.value;
                            vocab_obj["pronunciations"] = vocab.pronunciations;
                            if (vocab.intensity.has_value())
                            {
                                vocab_obj["intensity"] = vocab.intensity.value();
                            }
                            if (vocab.language.has_value())
                            {
                                vocab_obj["language"] = vocab.language.value();
                            }
                            vocab_array.push_back(vocab_obj);
                        }
                        j["custom_vocabulary_config"] = vocab_array;
                    }

                    // Callback
                    j["callback"] = callback;
                    if (callback && callback_config.has_value())
                    {
                        j["callback_config"] = nlohmann::json::parse(callback_config->toJson());
                    }

                    // Subtitles
                    j["subtitles"] = subtitles;
                    if (subtitles && subtitles_config.has_value())
                    {
                        j["subtitles_config"] = nlohmann::json::parse(subtitles_config->toString());
                    }

                    // Diarization
                    j["diarization"] = diarization;
                    if (diarization && diarization_config.has_value())
                    {
                        j["diarization_config"] = diarization_config->toJson();
                    }

                    // Translation
                    j["translation"] = translation;
                    if (translation)
                    {
                        j["translation_config"] = translation_config.toJson();
                    }

                    // Summarization
                    j["summarization"] = summarization;
                    if (summarization && summarization_config.has_value())
                    {
                        j["summarization_config"] = nlohmann::json::parse(summarization_config->toJson())["summarization_config"];
                    }

                    // Boolean flags
                    j["moderation"] = moderation;
                    j["named_entity_recognition"] = named_entity_recognition;
                    j["chapterization"] = chapterization;
                    j["name_consistency"] = name_consistency;
                    j["sentiment_analysis"] = sentiment_analysis;
                    j["sentences"] = sentences;
                    j["display_mode"] = display_mode;
                    j["punctuation_enhanced"] = punctuation_enhanced;

                    // Custom spelling
                    j["custom_spelling"] = custom_spelling;
                    if (custom_spelling && custom_spelling_config.has_value())
                    {
                        j["custom_spelling_config"] = custom_spelling_config->toJson()["spelling_dictionary"];
                    }

                    // Structured data extraction
                    j["structured_data_extraction"] = structured_data_extraction;
                    if (structured_data_extraction && structured_data_extraction_config.has_value())
                    {
                        j["structured_data_extraction_config"] = structured_data_extraction_config->toJson()["structured_data_extraction_config"];
                    }

                    // Audio to LLM
                    j["audio_to_llm"] = audio_to_llm;
                    if (audio_to_llm && audio_to_llm_config.has_value())
                    {
                        j["audio_to_llm_config"] = audio_to_llm_config->toJson()["prompts"];
                    }

                    // Language config
                    if (language_config.has_value())
                    {
                        j["language_config"] = language_config.value().toJson();
                    }

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