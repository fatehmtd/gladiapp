
#include "gladiapp_error.hpp"

using namespace gladiapp::v2::response;

void gladiapp::v2::response::TranscriptionError::reset()
{
    timestamp.clear();
    path.clear();
    request_id.clear();
    status_code = 0;
    message.clear();
    validation_errors.clear();
}

// TranscriptionError implementations
TranscriptionError TranscriptionError::fromJson(const std::string &jsonString)
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