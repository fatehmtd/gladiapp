#pragma once

#include "gladiapp_export.h"
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "gladiapp_ws_request.hpp"

namespace gladiapp
{
    namespace v2
    {
        // Forward declaration for the implementation details
        class GladiaWebsocketClientImpl;

        /**
         * WebSocket client for Gladia API.
         */
        class GLADIAPP_EXPORT GladiaWebsocketClient
        {
        public:
            GladiaWebsocketClient(const std::string &apiKey);

            bool connect(request::InitializeSessionRequest initRequest);
            void disconnect();

            bool startReceiving();

            bool sendAudio(const char *audioData, int size);

        private:
            std::unique_ptr<GladiaWebsocketClientImpl> _wsClientImpl;
        };
    }
}