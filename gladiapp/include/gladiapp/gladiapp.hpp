#pragma once

#include <string>

namespace gladiapp {
    namespace v2 {
        namespace headers {
            constexpr const char* X_GLADIA_KEY = "x-gladia-key";
        }

        namespace common {
            constexpr const char* USER_AGENT = "GladiaPP/1.0";
            constexpr const char* HOST = "api.gladia.io";
            constexpr const char* TRANSCRIPTION_ENDPOINT = "/v2/transcription";
            constexpr const char* UPLOAD_ENDPOINT = "/v2/upload";
        }
    }
}