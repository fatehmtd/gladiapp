#pragma once
#include <optional>
#include <nlohmann/json.hpp>

// nlohmann::json has no built-in support for std::optional<T>: assigning or
// reading an optional field requires an adl_serializer specialization, which
// upstream does not ship (vcpkg's nlohmann-json port carries an unofficial
// patch for it; a plain FetchContent build of upstream does not). This
// specialization lets `json["field"] = std::optional<T>{...}` and
// `j.get<std::optional<T>>()` work the way the rest of this codebase expects.
namespace nlohmann
{
    template <typename T>
    struct adl_serializer<std::optional<T>>
    {
        static void to_json(json &j, const std::optional<T> &opt)
        {
            if (opt.has_value())
            {
                j = *opt;
            }
            else
            {
                j = nullptr;
            }
        }

        static void from_json(const json &j, std::optional<T> &opt)
        {
            if (j.is_null())
            {
                opt.reset();
            }
            else
            {
                opt = j.get<T>();
            }
        }
    };
}
