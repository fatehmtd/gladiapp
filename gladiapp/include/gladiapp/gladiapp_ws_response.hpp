#pragma once
#include "gladiapp_export.h"
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace gladiapp
{
    namespace v2
    {
        namespace response {
            struct GLADIAPP_EXPORT InitializeSessionResponse {
                std::string id;
                std::string url;

                static InitializeSessionResponse fromJson(const nlohmann::json &json);
            };
        }
    }
}