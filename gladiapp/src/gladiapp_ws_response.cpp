#include "gladiapp_ws_response.hpp"

using namespace gladiapp::v2::response;

InitializeSessionResponse gladiapp::v2::response::InitializeSessionResponse::fromJson(const nlohmann::json &json)
{
    InitializeSessionResponse response;
    response.id = json.at("id").get<std::string>();
    response.url = json.at("url").get<std::string>();
    return response;
}
