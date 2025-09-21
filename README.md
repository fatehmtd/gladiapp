# GladiaPP

A C++ client library for the Gladia audio transcription API. Supports both REST API for pre-recorded audio and WebSocket for real-time transcription.

**Note**: This is a community project, not officially affiliated with Gladia.

## Features

- Audio upload and batch transcription (REST API)
- Real-time transcription (WebSocket)
- Multi-language translation
- Speaker diarization
- Subtitle generation (SRT/VTT)
- Named entity recognition
- Sentiment analysis and summarization
- Asynchronous operations

## Requirements

- C++17 or later
- CMake 3.16 or later
- Internet connection for API calls

### Dependencies

Dependencies are managed automatically via `vcpkg.json`:

- **Boost** (system, thread, beast) - HTTP client and networking
- **OpenSSL** - SSL/TLS support
- **nlohmann/json** - JSON parsing
- **spdlog** - Logging
- **base64** - Base64 encoding/decoding (fetched via CMake)

## Installation

The project uses vcpkg manifest mode for automatic dependency management.

### Build Steps

1. Install vcpkg if you haven't already:

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # On Windows: .\bootstrap-vcpkg.bat
```

2. Clone and build the project:

```bash
git clone https://github.com/fatehmtd/gladiapp.git
cd gladiapp
mkdir build && cd build
```

3. Configure with vcpkg (dependencies installed automatically):

```bash
# Linux/macOS
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake

# Windows
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake

# Optional: Build with examples
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_EXAMPLES=ON
```

4. Build:

```bash
cmake --build .
```

### Manual Dependencies (if not using vcpkg)

If you prefer to install dependencies manually:

- **macOS**: `brew install boost openssl cmake`
- **Ubuntu/Debian**: `sudo apt-get install libboost-all-dev libssl-dev cmake build-essential`
- **Windows**: Use Visual Studio with vcpkg integration, or install dependencies manually

## Quick Start

### REST API (Pre-recorded Audio)

```cpp
#include "gladiapp/gladiapp_rest.hpp"

int main() {
    gladiapp::v2::GladiaRestClient client("your-api-key");
    
    // Upload audio file
    auto uploadResponse = client.upload("/path/to/audio.wav");
    
    // Create transcription request
    gladiapp::v2::request::TranscriptionRequest request;
    request.audio_url = uploadResponse.audio_url;
    request.diarization = true;
    request.translation = true;
    request.translation_config.target_languages = {"fr", "es"};
    
    // Start transcription
    auto jobResponse = client.preRecorded(request);
    
    // Poll for completion
    while (true) {
        auto result = client.getResult(jobResponse.id);
        if (result.status == "done") break;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    
    return 0;
}
```

### WebSocket (Real-time Audio)

```cpp
#include "gladiapp/gladiapp_ws.hpp"

using namesapce gladiapp::v2::ws;

int main() {
    GladiaWebsocketClient client("your-api-key");
    
    // Configure session
    request::InitializeSessionRequest initRequest;
    initRequest.channels = 1;
    initRequest.bit_depth = request::InitializeSessionRequest::BitDepth::BIT_DEPTH_16;
    initRequest.encoding = request::InitializeSessionRequest::Encoding::WAV_PCM;
    initRequest.sample_rate = request::InitializeSessionRequest::SampleRate::SAMPLE_RATE_16000;
    
    // Create session
    auto session = client.connect(initRequest);
    if (session && session->connectAndStart()) {
        session->setOnTranscriptCallback([](const auto& transcript) {
            std::cout << transcript.data.utterance.text << std::endl;
        });
        
        // Send audio data
        std::vector<uint8_t> audioData = readAudioFile("audio.wav");
        session->sendAudioBinary(audioData.data(), audioData.size());
        
        session->sendStopSignal();
        session->disconnect();
    }
    
    return 0;
}
```

## API Usage

### REST API vs WebSocket

**REST API**: Use for pre-recorded audio files, batch processing, or when high accuracy is more important than speed.

**WebSocket**: Use for real-time transcription, live audio streams, or interactive applications requiring immediate feedback.

## Examples (Optional)

Working examples are provided in the `examples/` directory demonstrating both REST API and WebSocket functionality. Examples are optional and can be built separately from the main library.

### Building Examples

To build the examples, enable the `BUILD_EXAMPLES` option:

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_EXAMPLES=ON
cmake --build .
cmake --install . --prefix ./install
```

### API Key Setup

The examples use `dotenv-cpp` for secure API key management.

**Option 1: Environment Variable**

```bash
# Windows PowerShell
$env:GLADIA_API_KEY="your-api-key"

# Linux/macOS
export GLADIA_API_KEY="your-api-key"
```

**Option 2: .env File**

Create a `.env` file in the project root:

```env
GLADIA_API_KEY=your-api-key
```

See [`API_KEY_SETUP.md`](API_KEY_SETUP.md) for detailed instructions.

## API Reference

### GladiaRestClient

```cpp
GladiaRestClient(const std::string& apiKey);

// Upload audio file
UploadResponse upload(const std::string& filePath, TranscriptionError* error = nullptr);

// Create transcription job
PreRecordedResponse preRecorded(const TranscriptionRequest& request, TranscriptionError* error = nullptr);

// Get transcription result
ResultResponse getResult(const std::string& id, TranscriptionError* error = nullptr);

// List all results
ListResultsResponse getResults(const ListResultsQuery& query, TranscriptionError* error = nullptr);

// Delete a result
void deleteResult(const std::string& id, TranscriptionError* error = nullptr);
```

### GladiaWebsocketClient

```cpp
GladiaWebsocketClient(const std::string& apiKey);

// Create WebSocket session
std::shared_ptr<Session> connect(const InitializeSessionRequest& request, TranscriptionError* error = nullptr);

// Session methods
bool connectAndStart();
void sendAudioBinary(const uint8_t* data, int size);
void sendStopSignal();
void disconnect();

// Event callbacks
void setOnTranscriptCallback(std::function<void(const TranscriptResponse&)>);
void setOnTranslationCallback(std::function<void(const TranslationResponse&)>);
// ... other callbacks
```

### Configuration

**TranscriptionRequest**: `diarization`, `translation`, `subtitles`, `moderation`, `sentences`, `named_entity_recognition`, `sentiment_analysis`, `summarization`

**InitializeSessionRequest**: Audio format settings (`channels`, `bit_depth`, `encoding`, `sample_rate`), real-time processing options, post-processing settings

## Project Structure

```text
gladiapp/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── vcpkg.json                 # Dependency manifest
├── examples/                  # Optional examples (BUILD_EXAMPLES=ON)
│   ├── common/
│   │   └── apiKeyLoader.hpp   # API key management
│   ├── example-rest/          # REST API example
│   └── example-ws/            # WebSocket example
└── gladiapp/                  # Main library
    ├── include/gladiapp/      # Public headers
    └── src/                   # Implementation
```

## Contributing

Contributions are welcome! This is a community-driven project.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-feature`)
3. Commit your changes (`git commit -m 'Add new feature'`)
4. Push to the branch (`git push origin feature/new-feature`)
5. Open a Pull Request

### Guidelines

- Follow modern C++ practices (C++17+)
- Add tests for new features
- Update documentation as needed
- Ensure code compiles without warnings

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

- **Documentation**: Check header files for API details
- **Issues**: Report bugs via [GitHub Issues](https://github.com/fatehmtd/gladiapp/issues)
- **Discussions**: Join community discussions via [GitHub Discussions](https://github.com/fatehmtd/gladiapp/discussions)

---

**Note**: This project is not affiliated with Gladia. It is an independent, community-driven client library.
