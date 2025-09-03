#include <iostream>
#include <spdlog/spdlog.h>
#include <future>
#include <gladiapp/gladiapp_ws.hpp>
#include <gladiapp/gladiapp_error.hpp>

constexpr const char *tempApiKey = "a273934e-8ff7-4814-a41a-ba507614743d";

using namespace gladiapp::v2::ws;

int main(int ac, char **av)
{
    spdlog::info("Starting GladiaWebsocketClient...");
    gladiapp::v2::ws::GladiaWebsocketClient client(tempApiKey);

    gladiapp::v2::response::TranscriptionError error;
    gladiapp::v2::ws::request::InitializeSessionRequest initRequest;
    initRequest.channels = 1;
    initRequest.bit_depth = request::InitializeSessionRequest::BitDepth::BIT_DEPTH_16;
    initRequest.encoding = request::InitializeSessionRequest::Encoding::WAV_PCM;
    initRequest.region = request::InitializeSessionRequest::Region::US_WEST;

    client.connect(initRequest, &error);

    if(error.status_code != 0 ) {
        spdlog::error("Error occurred: {}", error.toString());
    }

    return 0;
}