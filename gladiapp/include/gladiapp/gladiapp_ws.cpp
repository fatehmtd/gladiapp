#include "gladiapp_ws.hpp"

using namespace gladiapp::v2::response;

InitializeSessionResponse gladiapp::v2::response::InitializeSessionResponse::fromJson(const nlohmann::json &json)
{
    InitializeSessionResponse response;
    response.id = json.at("id").get<std::string>();
    response.url = json.at("url").get<std::string>();
    return response;
}

nlohmann::json gladiapp::v2::request::InitializeSessionRequest::toJson() const
{
    nlohmann::json json;
    json["region"] = region;
    json["encoding"] = encoding;
    json["bitDepth"] = bitDepth;
    json["sampleRate"] = sampleRate;
    json["channels"] = channels;
    json["custom_metadata"] = custom_metadata;
    json["model"] = model;
    json["endpointing"] = endpointing;
    json["maximum_duration_without_endpointing"] = maximum_duration_without_endpointing;
    json["language_config"] = language_config.toJson();
    json["pre_processing"] = pre_processing.toJson();
    json["realtime_processing"] = realtime_processing.toJson();
    json["post_processing"] = post_processing.toJson();
    json["messages_config"] = messages_config.toJson();
    json["callback"] = callback;
    return json;
}
