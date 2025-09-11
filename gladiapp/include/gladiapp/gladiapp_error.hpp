#pragma once
#include "gladiapp_export.h"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace gladiapp
{
    namespace v2
    {
        namespace response
        {
            /**
             * Represents an error that occurs during transcription.
             * Contains information about the error's timestamp, path, and message.
             * Additionally, it includes the request ID, status code, and any validation errors.
             */
            struct GLADIAPP_EXPORT TranscriptionError
            {
                std::string timestamp;
                std::string path;
                std::string request_id;
                int status_code = 0;
                std::string message;
                std::vector<std::string> validation_errors;

                void reset();

                static TranscriptionError fromJson(const std::string &jsonString);

                std::string toString() const;
            };
        }
    }
}