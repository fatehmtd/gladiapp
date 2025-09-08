# GladiaPP - C++ Client Library for Gladia API

A modern C++ client library for interacting with the Gladia audio transcription API. This library provides both **REST API** and **WebSocket** (real-time) support for comprehensive audio transcription, translation, and analysis using the Gladia v2 API.

## ⚠️ Disclaimer

This is a **community-driven project** and is **not affiliated with Gladia in any way**. This library is developed and maintained by the community for educational and practical purposes. Gladia is a trademark of its respective owners.

## Features

### 🚀 Core Capabilities

- 🎵 **Audio Upload**: Upload audio files directly to Gladia's servers
- 📝 **Pre-recorded Transcription**: Create and manage batch transcription jobs
- ⚡ **Real-time Transcription**: Live audio transcription via WebSocket
- 🌍 **Translation**: Support for multi-language translation (real-time and post-processing)
- 👥 **Speaker Diarization**: Identify different speakers in audio
- 📱 **Subtitles**: Generate SRT and VTT subtitle formats
- 🔍 **Content Moderation**: Built-in content moderation capabilities

### 🧠 Advanced AI Features

- 🏷️ **Named Entity Recognition**: Extract names, places, organizations, and other entities
- 😊 **Sentiment Analysis**: Analyze emotional tone and sentiment
- 📋 **Summarization**: Generate concise summaries of transcriptions
- ⏱️ **Sentence Timestamps**: Detailed timing information at sentence level
- 🎯 **Speech Events**: Real-time speech start/end detection

### 🔧 Technical Features

- 📊 **Async Support**: Asynchronous operations for better performance
- 🛡️ **Error Handling**: Comprehensive error handling and validation
- 🔄 **Session Management**: Full WebSocket session lifecycle management
- 📡 **Event Callbacks**: Rich callback system for real-time events
- 🎛️ **Configurable Processing**: Flexible real-time and post-processing options

## Requirements

- **C++17** or later
- **CMake 3.16** or later
- **Boost** (system, thread, beast components)
- **OpenSSL**
- **Internet connection** for API calls

### Dependencies (automatically fetched)

- [nlohmann/json](https://github.com/nlohmann/json) - JSON parsing
- [spdlog](https://github.com/gabime/spdlog) - Logging
- [base64](https://github.com/tobiaslocker/base64) - Base64 encoding/decoding

## Installation

### Using CMake

1. Clone the repository:

```bash
git clone https://github.com/fatehmtd/gladiapp.git
cd gladiapp
```

2. Create a build directory:

```bash
mkdir build && cd build
```

3. Configure and build:

```bash
cmake ..
make
```

### System Dependencies

#### macOS (using Homebrew)

```bash
brew install boost openssl cmake
```

#### Ubuntu/Debian

```bash
sudo apt-get install libboost-all-dev libssl-dev cmake build-essential
```

#### CentOS/RHEL

```bash
sudo yum install boost-devel openssl-devel cmake gcc-c++
```

#### Windows

**Using vcpkg (Recommended):**

```powershell
# Install vcpkg if you haven't already
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install dependencies
.\vcpkg install boost openssl cmake
```

**Using Visual Studio:**

- Install Visual Studio 2019/2022 with C++ development tools
- Use vcpkg integration: `.\vcpkg integrate install`
- Open the project with CMake support in Visual Studio

**Using Visual Studio Code:**

- Install Visual Studio Code
- Install the following extensions:
  - C/C++ Extension Pack
  - CMake Tools
  - CMake Language Support
- Install dependencies via vcpkg or Chocolatey (see above)
- Open the project folder in VS Code
- Configure CMake kit when prompted
- Use Ctrl+Shift+P → "CMake: Configure" to configure the project
- Use Ctrl+Shift+P → "CMake: Build" to build the project

**Build on Windows:**

```powershell
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Quick Start

### REST API Usage (Pre-recorded Audio)

```cpp
#include "gladiapp/gladiapp_rest.hpp"
#include <future>

int main() {
    // Initialize client with your API key
    gladiapp::v2::GladiaRestClient client("your-api-key-here");
    
    try {
        // Upload audio file
        auto uploadResponse = client.upload("/path/to/your/audio.wav");
        
        // Create transcription request
        gladiapp::v2::request::TranscriptionRequest request;
        request.audio_url = uploadResponse.audio_url;
        request.diarization = true;
        request.subtitles = true;
        request.translation = true;
        request.translation_config.target_languages = {"fr", "es"};
        
        // Start transcription job
        auto jobResponse = client.preRecorded(request);
        
        // Poll for results
        std::string status = "";
        while (status != "done" && status != "error") {
            auto result = client.getResult(jobResponse.id);
            status = result.status;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        
        std::cout << "Transcription completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### WebSocket Usage (Real-time Audio)

```cpp
#include "gladiapp/gladiapp_ws.hpp"
#include <vector>

int main() {
    // Initialize WebSocket client
    gladiapp::v2::ws::GladiaWebsocketClient client("your-api-key-here");
    
    // Configure session
    gladiapp::v2::ws::request::InitializeSessionRequest initRequest;
    initRequest.channels = 1;
    initRequest.bit_depth = gladiapp::v2::ws::request::InitializeSessionRequest::BitDepth::BIT_DEPTH_16;
    initRequest.encoding = gladiapp::v2::ws::request::InitializeSessionRequest::Encoding::WAV_PCM;
    initRequest.sample_rate = gladiapp::v2::ws::request::InitializeSessionRequest::SampleRate::SAMPLE_RATE_16000;
    
    // Enable real-time features
    gladiapp::v2::ws::request::InitializeSessionRequest::RealtimeProcessing realtimeProcessing;
    realtimeProcessing.translation = true;
    realtimeProcessing.named_entity_recognition = true;
    initRequest.realtime_processing = realtimeProcessing;
    
    // Create and connect session
    auto session = client.connect(initRequest);
    if (session && session->connectAndStart()) {
        
        // Set up callbacks
        session->setOnTranscriptCallback([](const auto& transcript) {
            std::cout << "Transcript: " << transcript.data.utterance.text << std::endl;
        });
        
        session->setOnTranslationCallback([](const auto& translation) {
            if (translation.data.has_value()) {
                std::cout << "Translation: " << translation.data->translated_utterance.text << std::endl;
            }
        });
        
        // Send audio data
        std::vector<uint8_t> audioData = readAudioFile("audio.wav");
        session->sendAudioBinary(audioData.data(), audioData.size());
        
        // Process results...
        session->sendStopSignal();
        session->disconnect();
    }
    
    return 0;
}
```

### Async Upload Example

```cpp
// Perform async upload while doing other work
std::future<gladiapp::v2::response::UploadResponse> uploadFuture = 
    std::async([&client]() {
        return client.upload("/path/to/audio.wav");
    });

// Do other work here...

// Get upload result
auto uploadResponse = uploadFuture.get();
```

## Usage Patterns

### When to Use REST API vs WebSocket

**Use REST API for:**

- 📁 **Pre-recorded audio files** that you want to transcribe in batch
- 🎯 **High accuracy requirements** where processing time is not critical
- 🔄 **Offline processing** scenarios
- 📊 **Complex post-processing** like detailed analytics and reporting
- 💾 **Archival transcriptions** that will be stored and retrieved later

**Use WebSocket for:**

- ⚡ **Real-time transcription** during live conversations or meetings
- 🎙️ **Live streaming audio** from microphones or audio inputs
- 💬 **Interactive applications** that need immediate feedback
- 🎮 **Voice-controlled applications** and games
- 📱 **Live captioning** and accessibility features

### Hybrid Usage

You can also combine both approaches in a single application:

```cpp
// Use WebSocket for real-time preview
auto wsSession = wsClient.connect(realtimeConfig);
wsSession->setOnTranscriptCallback([](const auto& transcript) {
    // Show live captions
    displayLiveCaption(transcript.data.utterance.text);
});

// Use REST API for final high-quality processing
auto uploadResponse = restClient.upload(recordedAudioFile);
gladiapp::v2::request::TranscriptionRequest request;
request.audio_url = uploadResponse.audio_url;
request.diarization = true;
request.summarization = true;
// Get final, high-quality transcription with full features
auto job = restClient.preRecorded(request);
```

## Examples

The repository includes comprehensive working examples that demonstrate all the major features of the GladiaPP library. Both REST API and WebSocket examples are provided in the `examples/` directory.

### REST API Example

The REST API example is located in `examples/example-rest/` and demonstrates pre-recorded audio transcription with comprehensive features.

### WebSocket Example

The WebSocket example is located in `examples/example-ws/` and demonstrates real-time audio transcription with live processing capabilities.

#### Building and Running the Examples

1. **Build the project** (if you haven't already):

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

1. **Install the examples** to the install directory:

```bash
cmake --install . --prefix ./install
```

1. **Run the examples**:

```bash
cd install/bin
./run_example.sh
```

#### What the REST Example Demonstrates

The REST example (`examples/example-rest/main.cpp`) showcases:

- **🎵 Async Audio Upload**: Uploads a test audio file (`testing.wav`) asynchronously while performing other tasks
- **📝 Transcription Job Creation**: Creates a comprehensive transcription job with multiple features enabled
- **👥 Speaker Diarization**: Identifies different speakers in the audio
- **🛡️ Content Moderation**: Applies content moderation to the transcription
- **📱 Subtitle Generation**: Generates both VTT and SRT subtitle formats
- **📊 Sentence-level Timestamps**: Provides detailed timing information
- **🌍 Multi-language Translation**: Translates the transcription to French, German, and Spanish
- **🔄 Job Polling**: Polls the transcription status until completion
- **📋 Results Management**: Lists all transcription results and cleans them up

#### What the WebSocket Example Demonstrates

The WebSocket example (`examples/example-ws/main.cpp`) showcases:

- **⚡ Real-time Transcription**: Live audio transcription as audio is being processed
- **🌍 Real-time Translation**: Instant translation to multiple languages (French, Spanish)
- **🏷️ Named Entity Recognition**: Real-time extraction of entities from speech
- **📋 Summarization**: Post-processing summarization of the complete transcript
- **🎯 Speech Events**: Detection of speech start/end events
- **📡 Event Callbacks**: Rich callback system for handling different types of events
- **🔄 Session Management**: Complete WebSocket session lifecycle
- **🎛️ Configurable Processing**: Flexible real-time and post-processing configuration

#### Example Features Configuration

**REST API Configuration:**

```cpp
gladiapp::v2::request::TranscriptionRequest request;
request.audio_url = response.audio_url;
request.diarization = true;                    // Speaker identification
request.moderation = true;                     // Content moderation
request.subtitles = true;                      // Subtitle generation
request.sentences = true;                      // Sentence timestamps

// Configure subtitle formats
request.subtitles_config = gladiapp::v2::request::TranscriptionRequest::SubtitlesConfig();
request.subtitles_config->formats = {
    gladiapp::v2::request::TranscriptionRequest::SubtitlesConfig::Format::VTT,
    gladiapp::v2::request::TranscriptionRequest::SubtitlesConfig::Format::SRT
};

// Configure translation
request.translation = true;
request.translation_config.target_languages = {"fr", "de", "es"};
```

**WebSocket Configuration:**

```cpp
gladiapp::v2::ws::request::InitializeSessionRequest initRequest;
initRequest.channels = 1;
initRequest.bit_depth = request::InitializeSessionRequest::BitDepth::BIT_DEPTH_16;
initRequest.encoding = request::InitializeSessionRequest::Encoding::WAV_PCM;
initRequest.sample_rate = request::InitializeSessionRequest::SampleRate::SAMPLE_RATE_16000;

// Real-time processing
request::InitializeSessionRequest::RealtimeProcessing realtimeProcessing;
realtimeProcessing.translation = true;
realtimeProcessing.named_entity_recognition = true;

// Translation configuration
request::InitializeSessionRequest::RealtimeProcessing::TranslationConfig translationConfig;
translationConfig.target_languages = {"fr", "es"};
realtimeProcessing.translation_config = translationConfig;

// Post processing
request::InitializeSessionRequest::PostProcessing postProcessing;
postProcessing.summarization = true;
initRequest.post_processing = postProcessing;
```

#### Example Output

When you run the example, you'll see output similar to:

```text
[2024-08-30 10:30:15] [info] Upload started, doing other work...
[2024-08-30 10:30:18] [info] response: {"audio_url":"https://api.gladia.io/v2/upload/abc123..."}
[2024-08-30 10:30:19] [info] Transcription job created, ID: job_abc123...
[2024-08-30 10:30:24] [info] Transcription result: queued
[2024-08-30 10:30:29] [info] Transcription result: processing
[2024-08-30 10:30:34] [info] Transcription result: done
[2024-08-30 10:30:35] [info] getting transcription results...
[2024-08-30 10:30:35] [info] deleting result: job_abc123...
```

#### Required Files

The example includes:

- **`main.cpp`**: The main example source code
- **`testing.wav`**: A sample audio file for testing
- **`CMakeLists.txt`**: Build configuration for the example

#### API Key Note

⚠️ **Important**: The example uses a temporary API key for demonstration purposes. For production use, you should:

1. **Get your own API key** from [Gladia](https://gladia.io)
2. **Replace the temporary key** in `main.cpp`:

```cpp
// Replace this line
constexpr const char *tempApiKey = "temporary-demo-key";

// With your actual API key
constexpr const char *tempApiKey = "your-actual-api-key";
```

3. **Use environment variables** for better security:

```cpp
#include <cstdlib>

int main(int ac, char **av) {
    const char* apiKey = std::getenv("GLADIA_API_KEY");
    if (!apiKey) {
        std::cerr << "Please set GLADIA_API_KEY environment variable" << std::endl;
        return 1;
    }
    
    gladiapp::v2::GladiaRestClient client(apiKey);
    // ... rest of the code
}
```

Then run with:

```bash
export GLADIA_API_KEY="your-actual-api-key"
./run_example.sh
```

## API Reference

### GladiaRestClient (Pre-recorded Audio)

#### Constructor

```cpp
GladiaRestClient(const std::string& apiKey);
```

#### Methods

- `upload(const std::string& filePath)` - Upload audio file
- `preRecorded(const TranscriptionRequest& request, TranscriptionError* error = nullptr)` - Create transcription job
- `getResult(const std::string& id, TranscriptionError* error = nullptr)` - Get transcription result
- `getResults(const ListResultsQuery& query, TranscriptionError* error = nullptr)` - List all results
- `deleteResult(const std::string& id, TranscriptionError* error = nullptr)` - Delete a result

### GladiaWebsocketClient (Real-time Audio)

#### WebSocket Constructor

```cpp
GladiaWebsocketClient(const std::string& apiKey);
```

#### WebSocket Methods

- `connect(const InitializeSessionRequest& request, TranscriptionError* error = nullptr)` - Create WebSocket session
- Session methods:
  - `connectAndStart()` - Start the WebSocket connection
  - `sendAudioBinary(const uint8_t* data, int size)` - Send audio data
  - `sendStopSignal()` - Signal end of audio stream
  - `disconnect()` - Close the connection

#### Event Callbacks

- `setOnConnectedCallback()` - Connection established
- `setOnSpeechStartedCallback()` - Speech detection started
- `setOnSpeechEndedCallback()` - Speech detection ended
- `setOnTranscriptCallback()` - Transcription results (partial/final)
- `setOnTranslationCallback()` - Translation results
- `setOnNamedEntityRecognitionCallback()` - Named entity recognition results
- `setOnPostTranscriptCallback()` - Post-processing transcript
- `setOnFinalTranscriptCallback()` - Final complete transcript

### Configuration Options

#### TranscriptionRequest Features (REST)

- `diarization` - Speaker identification
- `translation` - Multi-language translation
- `subtitles` - Subtitle generation (SRT/VTT)
- `moderation` - Content moderation
- `sentences` - Sentence-level timestamps
- `named_entity_recognition` - Extract named entities
- `sentiment_analysis` - Analyze sentiment
- `summarization` - Generate summaries

#### InitializeSessionRequest Features (WebSocket)

- **Audio Configuration**: channels, bit_depth, encoding, sample_rate
- **Real-time Processing**: translation, named_entity_recognition
- **Post Processing**: summarization
- **Messages Configuration**: Fine-grained control over what events to receive

## Project Structure

```text
gladiapp/
├── CMakeLists.txt              # Root CMake configuration
├── README.md                   # This file
├── LICENSE                     # MIT License file
├── build/                      # Build artifacts (generated)
├── examples/                   # Example applications
│   ├── example-rest/          # REST API example (pre-recorded)
│   │   ├── CMakeLists.txt     # Example build configuration
│   │   ├── main.cpp           # REST example source code
│   │   └── testing.wav        # Sample audio file for testing
│   └── example-ws/            # WebSocket example (real-time)
│       ├── CMakeLists.txt     # Example build configuration
│       ├── main.cpp           # WebSocket example source code
│       └── testing.wav        # Sample audio file for testing
└── gladiapp/                  # Main library source
    ├── CMakeLists.txt         # Library CMake configuration
    ├── include/               # Header files
    │   └── gladiapp/
    │       ├── gladiapp.hpp           # Main library header
    │       ├── gladiapp_error.hpp     # Error handling
    │       ├── gladiapp_rest.hpp      # REST API client header
    │       ├── gladiapp_rest_request.hpp  # REST request structures
    │       ├── gladiapp_rest_response.hpp # REST response structures
    │       ├── gladiapp_ws.hpp        # WebSocket client header
    │       ├── gladiapp_ws_request.hpp    # WebSocket request structures
    │       ├── gladiapp_ws_response.hpp   # WebSocket response structures
    │       ├── utils.hpp              # Utility functions
    │       └── impl/                  # Implementation details
    └── src/                   # Implementation files
        ├── gladiapp_error.cpp         # Error handling implementation
        ├── gladiapp_rest.cpp          # REST client implementation
        ├── gladiapp_rest_request.cpp  # REST request structures
        ├── gladiapp_rest_response.cpp # REST response structures
        ├── gladiapp_ws.cpp            # WebSocket client implementation
        ├── gladiapp_ws_request.cpp    # WebSocket request structures
        └── gladiapp_ws_response.cpp   # WebSocket response structures
```

## Contributing

We welcome contributions! This is a community-driven project, and we appreciate any help to improve it.

### How to Contribute

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### Development Guidelines

- Follow modern C++ best practices (C++17+)
- Add tests for new features
- Update documentation as needed
- Ensure code compiles without warnings
- Use meaningful commit messages

## TODO

- [ ] **Testing Suite**
  - [ ] Unit tests for REST API endpoints
  - [ ] Unit tests for WebSocket functionality
  - [ ] Integration tests with mock server
  - [ ] Performance benchmarks for both REST and WebSocket

- [ ] **Enhanced Features**
  - [ ] Connection pooling for REST client
  - [ ] Automatic reconnection for WebSocket sessions
  - [ ] Batch processing capabilities
  - [ ] Custom vocabulary configuration
  - [ ] Audio format validation and conversion
  - [ ] Streaming uploads for large files

- [ ] **Documentation**
  - [ ] Complete API documentation with Doxygen
  - [ ] More usage examples for advanced features
  - [ ] Tutorial for beginners
  - [ ] Performance optimization guide
  - [ ] WebSocket best practices guide

- [ ] **Error Handling & Reliability**
  - [ ] Better error messages with context
  - [ ] Retry mechanisms with exponential backoff
  - [ ] Rate limiting handling
  - [ ] Network timeout configuration
  - [ ] Graceful degradation strategies

- [ ] **Performance Optimizations**
  - [ ] Memory optimization for large audio files
  - [ ] Async/await pattern improvements
  - [ ] WebSocket message queuing
  - [ ] Compression support for audio data

- [ ] **Developer Experience**
  - [ ] CMake package configuration
  - [ ] vcpkg package support
  - [ ] Conan package support
  - [ ] Pre-built binaries for common platforms

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

### Why MIT License?

- ✅ **Free for everyone** - Use in personal, commercial, or open source projects
- ✅ **Simple and permissive** - No complex legal requirements
- ✅ **Maximum adoption** - Widely trusted by developers and companies
- ✅ **Community-friendly** - Encourages contributions and collaboration

Feel free to use GladiaPP in your C++ projects! Just include the copyright notice and license text.

## Support

- 📄 **Documentation**: Check the header files for detailed API documentation
- 🐛 **Issues**: Report bugs or request features via [GitHub Issues](https://github.com/fatehmtd/gladiapp/issues)
- 💬 **Discussions**: Join community discussions in [GitHub Discussions](https://github.com/fatehmtd/gladiapp/discussions)

## Acknowledgments

- **Gladia Team** - For providing the excellent transcription API
- **nlohmann/json** - For the amazing JSON library
- **spdlog** - For the fast logging library
- **Boost Libraries** - For networking and threading support
- **Community Contributors** - Thank you for making this project better!

---

**Note**: This project is not affiliated with, endorsed by, or connected to Gladia. It is an independent, community-driven client library for educational and practical use.
