#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <memory>

#include "gladiapp_export.h"
#include "gladiapp_error.hpp"

namespace gladiapp
{
    namespace v2
    {
        namespace response
        {
            /**
             * Represents the response for an audio request.
             * Contains the audio file's URL and its metadata.
             */
            struct GLADIAPP_EXPORT UploadResponse
            {
                std::string audio_url;

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

                    static AudioMetadata fromJson(const std::string &jsonString);

                    std::string toString() const;
                };

                AudioMetadata audio_metadata;

                static UploadResponse fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            /**
             * Represents the response for a transcription job request.
             * Contains the transcription's ID and the URL to the result.
             */
            struct GLADIAPP_EXPORT TranscriptionJobResponse
            {
                std::string id;
                std::string result_url;

                static TranscriptionJobResponse fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            /**
             * Represents an error that occurs during sentence processing.
             * Contains information about the error's status code, exception type, and message.
             */
            struct GLADIAPP_EXPORT SentenceError
            {
                int status_code;
                std::string exception;
                std::string message;

                static SentenceError fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            /**
             * Represents a file to be transcribed.
             * Contains information about the file's ID and filename.
             * Additionally, it includes the file's source, duration, and number of channels.
             */
            struct GLADIAPP_EXPORT TranscriptionFile
            {
                std::string id;
                std::string filename;
                std::string source;
                double duration;
                int number_of_channels;

                static TranscriptionFile fromJson(const std::string &jsonString);

                std::string toString() const;
            };

            /**
             * Represents the metadata for a transcription result.
             * Contains information about the audio duration and number of distinct channels.
             */
            struct GLADIAPP_EXPORT Metadata
            {
                double audio_duration;
                int number_of_distinct_channels;
                double billing_time;
                double transcription_time;

                static Metadata fromJson(const std::string &jsonString);
            };

            /**
             * Represents the result of a transcription job.
             * Contains information about the job's ID, request ID, and other metadata.
             * Additionally, it includes the job's status, created_at timestamp, and any error information.
             */
            struct GLADIAPP_EXPORT TranscriptionResult
            {
                std::string id;
                std::string request_id;
                int version;
                std::string status;
                std::string created_at;
                std::string kind;
                std::optional<std::string> completed_at;
                std::optional<int> error_code;

                std::optional<TranscriptionFile> file;
                std::string request_params;

                /**
                 * Represents the object containing the transcription result.
                 * Contains metadata and the actual transcription result.
                 */
                struct TranscriptionObject
                {
                    Metadata metadata;

                    /**
                     * Represents the result of a transcription job.
                     * Contains the full transcript and any detected languages.
                     */
                    struct Result
                    {
                        std::string full_transcript;
                        std::vector<std::string> languages;
                        struct Utterance
                        {
                            std::string language;
                            double start;
                            double end;
                            double confidence;
                            int channel;

                            struct Word
                            {
                                std::string word;
                                double start;
                                double end;
                                double confidence;

                                static Word fromJson(const std::string &jsonString);

                                std::string toString() const;
                            };

                            std::vector<Word> words;
                            std::string text;
                            int speaker;

                            static Utterance fromJson(const std::string &jsonString);

                            std::string toString() const;
                        };
                        std::vector<Utterance> utterances;

                        struct Subtitle
                        {
                            std::string format;
                            std::string subtitles;
                            static Subtitle fromJson(const std::string &jsonString);
                        };
                        std::vector<Subtitle> subtitles;

                        static Result fromJson(const std::string &jsonString);
                    };

                    Result result;

                    static TranscriptionObject fromJson(const std::string &jsonString);
                };
                TranscriptionObject result;

                struct Moderation
                {
                    bool success;
                    bool is_empty;
                    std::string results;
                    double exec_time;
                    std::optional<std::string> error;

                    static Moderation fromJson(const std::string &jsonString);
                };

                static TranscriptionResult fromJson(const std::string &jsonString);
            };

            /**
             * Represents the pagination information for a list of results.
             * Contains the URLs for the first, current, and next pages of results.
             */
            struct GLADIAPP_EXPORT TranscriptionListResults
            {
                std::string first;
                std::string current;
                std::optional<std::string> next;
                std::vector<TranscriptionResult> items;
                static TranscriptionListResults fromJson(const std::string &jsonString);
            };
        }
    }
}