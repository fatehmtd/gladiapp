#include "gladiapp/gladiapp_rest_response.hpp"
#include <algorithm>

namespace gladiapp
{
    namespace v2
    {
        namespace response
        {
            // AudioMetadata implementations
            UploadResponse::AudioMetadata UploadResponse::AudioMetadata::fromJson(const std::string &jsonString)
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

            std::string UploadResponse::AudioMetadata::toString() const
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

            // UploadResponse implementations
            UploadResponse UploadResponse::fromJson(const std::string &jsonString)
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

            std::string UploadResponse::toString() const
            {
                nlohmann::json j;
                j["audio_url"] = audio_url;
                j["audio_metadata"] = nlohmann::json::parse(audio_metadata.toString());
                return j.dump();
            }

            // TranscriptionJobResponse implementations
            TranscriptionJobResponse TranscriptionJobResponse::fromJson(const std::string &jsonString)
            {
                TranscriptionJobResponse response;
                auto json = nlohmann::json::parse(jsonString);
                response.id = json.value("id", "");
                response.result_url = json.value("result_url", "");
                return response;
            }

            std::string TranscriptionJobResponse::toString() const
            {
                nlohmann::json j;
                j["id"] = id;
                j["result_url"] = result_url;
                return j.dump();
            }

            std::string TranscriptionError::toString() const
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

            // TranscriptionFile implementations
            TranscriptionFile TranscriptionFile::fromJson(const std::string &jsonString)
            {
                TranscriptionFile file;
                auto json = nlohmann::json::parse(jsonString);

                file.id = json.value("id", "");
                file.filename = json.value("filename", "");
                if (json.contains("source") && json["source"].is_string())
                {
                    file.source = json.value("source", "");
                }
                if(json.contains("duration") && json["duration"].is_number()) {
                    file.duration = json.value("duration", 0.0);
                } else if(json.contains("audio_duration") && json["audio_duration"].is_number()) {
                    file.duration = json.value("audio_duration", 0.0);
                } else {
                    file.duration = 0.0;
                }
                file.number_of_channels = json.value("number_of_channels", 0);

                return file;
            }

            std::string TranscriptionFile::toString() const
            {
                nlohmann::json j;
                j["id"] = id;
                j["filename"] = filename;
                j["source"] = source;
                j["duration"] = duration;
                j["number_of_channels"] = number_of_channels;
                return j.dump();
            }

            // SentenceError implementations
            SentenceError SentenceError::fromJson(const std::string &jsonString)
            {
                SentenceError error;
                auto json = nlohmann::json::parse(jsonString);

                error.status_code = json.value("status_code", 0);
                error.exception = json.value("exception", "");
                error.message = json.value("message", "");

                return error;
            }

            std::string SentenceError::toString() const
            {
                nlohmann::json j;
                j["status_code"] = status_code;
                j["exception"] = exception;
                j["message"] = message;
                return j.dump();
            }

            // Word implementations
            TranscriptionResult::TranscriptionObject::Result::Utterance::Word TranscriptionResult::TranscriptionObject::Result::Utterance::Word::fromJson(const std::string &jsonString)
            {
                Word word;
                auto json = nlohmann::json::parse(jsonString);

                word.word = json.value("word", "");
                word.start = json.value("start", 0.0);
                word.end = json.value("end", 0.0);
                word.confidence = json.value("confidence", 0.0);

                return word;
            }

            std::string TranscriptionResult::TranscriptionObject::Result::Utterance::Word::toString() const
            {
                nlohmann::json j;
                j["word"] = word;
                j["start"] = start;
                j["end"] = end;
                j["confidence"] = confidence;
                return j.dump();
            }

            // Utterance implementations
            TranscriptionResult::TranscriptionObject::Result::Utterance TranscriptionResult::TranscriptionObject::Result::Utterance::fromJson(const std::string &jsonString)
            {
                Utterance utterance;
                auto json = nlohmann::json::parse(jsonString);

                utterance.language = json.value("language", "");
                utterance.start = json.value("start", 0.0);
                utterance.end = json.value("end", 0.0);
                utterance.confidence = json.value("confidence", 0.0);
                utterance.channel = json.value("channel", 0);
                if (json.contains("words"))
                {
                    for (const auto &wordJson : json["words"])
                    {
                        utterance.words.push_back(Word::fromJson(wordJson.dump()));
                    }
                }
                utterance.text = json.value("text", "");
                utterance.speaker = json.value("speaker", 0);

                return utterance;
            }

            std::string TranscriptionResult::TranscriptionObject::Result::Utterance::toString() const
            {
                nlohmann::json j;
                j["language"] = language;
                j["start"] = start;
                j["end"] = end;
                j["confidence"] = confidence;
                j["channel"] = channel;
                j["words"] = nlohmann::json::array();
                for (const auto &word : words)
                {
                    j["words"].push_back(word.toString());
                }
                j["text"] = text;
                j["speaker"] = speaker;
                return j.dump();
            }

            // Subtitle implementations
            TranscriptionResult::TranscriptionObject::Result::Subtitle TranscriptionResult::TranscriptionObject::Result::Subtitle::fromJson(const std::string &jsonString)
            {
                Subtitle subtitle;
                auto json = nlohmann::json::parse(jsonString);
                return subtitle;
            }

            // Result implementations
            TranscriptionResult::TranscriptionObject::Result TranscriptionResult::TranscriptionObject::Result::fromJson(const std::string &jsonString)
            {
                Result result;
                auto json = nlohmann::json::parse(jsonString);
                if (json.contains("full_transcript"))
                {
                    result.full_transcript = json.value("full_transcript", "");
                }
                if (json.contains("languages"))
                {
                    result.languages = json.value("languages", std::vector<std::string>{});
                }
                if (json.contains("utterances"))
                {
                    for (const auto &utteranceJson : json["utterances"])
                    {
                        result.utterances.push_back(Utterance::fromJson(utteranceJson.dump()));
                    }
                }
                if (json.contains("subtitles"))
                {
                    for (const auto &subtitleJson : json["subtitles"])
                    {
                        result.subtitles.push_back(Subtitle::fromJson(subtitleJson.dump()));
                    }
                }
                return result;
            }

            // Metadata implementations
            Metadata Metadata::fromJson(const std::string &jsonString)
            {
                Metadata metadata;
                auto json = nlohmann::json::parse(jsonString);

                metadata.audio_duration = json.value("audio_duration", 0.0);
                metadata.number_of_distinct_channels = json.value("number_of_distinct_channels", 0);
                metadata.billing_time = json.value("billing_time", 0.0);
                metadata.transcription_time = json.value("transcription_time", 0.0);

                return metadata;
            }

            // TranscriptionObject implementations
            TranscriptionResult::TranscriptionObject TranscriptionResult::TranscriptionObject::fromJson(const std::string &jsonString)
            {
                TranscriptionObject obj;
                auto json = nlohmann::json::parse(jsonString);
                if (json.contains("metadata"))
                {
                    obj.metadata = Metadata::fromJson(json["metadata"].dump());
                }
                if (json.contains("result"))
                {
                    obj.result = Result::fromJson(json["result"].dump());
                }
                return obj;
            }

            // Moderation implementations
            TranscriptionResult::Moderation TranscriptionResult::Moderation::fromJson(const std::string &jsonString)
            {
                Moderation moderation;
                auto json = nlohmann::json::parse(jsonString);
                moderation.success = json.value("success", false);
                moderation.is_empty = json.value("is_empty", false);
                moderation.results = json.value("results", "");
                moderation.exec_time = json.value("exec_time", 0.0);
                if (json.contains("error") && !json["error"].is_null())
                {
                    moderation.error = json.value("error", "");
                }
                return moderation;
            }

            // TranscriptionResult implementations
            TranscriptionResult TranscriptionResult::fromJson(const std::string &jsonString)
            {
                TranscriptionResult result;
                auto json = nlohmann::json::parse(jsonString);
                if (json.contains("id"))
                {
                    result.id = json.value("id", "");
                }
                if (json.contains("request_id"))
                {
                    result.request_id = json.value("request_id", "");
                }
                if (json.contains("version"))
                {
                    result.version = json.value("version", 0);
                }
                if (json.contains("status"))
                {
                    result.status = json.value("status", "");
                }
                if (json.contains("created_at") && json["created_at"].is_string())
                {
                    result.created_at = json.value("created_at", "");
                }
                if (json.contains("kind"))
                {
                    result.kind = json.value("kind", "");
                }
                if (json.contains("completed_at") && json["completed_at"].is_string())
                {
                    result.completed_at = json.value("completed_at", "");
                }
                if (json.contains("error_code") && json["error_code"].is_number())
                {
                    result.error_code = json.value("error_code", 0);
                }
                if (json.contains("file") && json["file"].is_object())
                {
                    result.file = TranscriptionFile::fromJson(json["file"].dump());
                }
                if (json.contains("request_params") && json["request_params"].is_object())
                {
                    result.request_params = json["request_params"].dump();
                }
                if (json.contains("result") && json["result"].is_object())
                {
                    result.result = TranscriptionObject::fromJson(json["result"].dump());
                }
                return result;
            }
        }
    }
}

gladiapp::v2::response::TranscriptionListResults gladiapp::v2::response::TranscriptionListResults::fromJson(const std::string &jsonString)
{
    TranscriptionListResults results;
    auto json = nlohmann::json::parse(jsonString);
    results.first = json.value("first", "");
    results.current = json.value("current", "");
    if(json.contains("next") && !json["next"].is_null()) {
        results.next = json.value("next", "");
    }
    if(json.contains("items") && json["items"].is_array()) {
        for (const auto &item : json["items"])
        {
            results.items.push_back(TranscriptionResult::fromJson(item.dump()));
        }
    }
    return results;
}