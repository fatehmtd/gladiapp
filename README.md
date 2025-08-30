# GladiaPP - C++ Client Library for Gladia API

A modern C++ client library for interacting with the Gladia audio transcription API. This library provides a simple and efficient way to upload audio files, create transcription jobs, and retrieve results using the Gladia v2 API.

## ⚠️ Disclaimer

This is a **community-driven project** and is **not affiliated with Gladia in any way**. This library is developed and maintained by the community for educational and practical purposes. Gladia is a trademark of its respective owners.

## Features

- 🎵 **Audio Upload**: Upload audio files directly to Gladia's servers
- 📝 **Transcription**: Create and manage transcription jobs
- 🌍 **Translation**: Support for multi-language translation
- 👥 **Speaker Diarization**: Identify different speakers in audio
- 📱 **Subtitles**: Generate SRT and VTT subtitle formats
- 🔍 **Moderation**: Content moderation capabilities
- 📊 **Async Support**: Asynchronous operations for better performance
- 🛡️ **Error Handling**: Comprehensive error handling and validation

## Requirements

- **C++17** or later
- **CMake 3.16** or later
- **Boost** (system, thread components)
- **OpenSSL**
- **Internet connection** for API calls

### Dependencies (automatically fetched)

- [nlohmann/json](https://github.com/nlohmann/json) - JSON parsing
- [spdlog](https://github.com/gabime/spdlog) - Logging

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

### Basic Usage

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

## Examples

### REST API Example

The repository includes a complete working example that demonstrates all the major features of the GladiaPP library. The example is located in the `examples/example-rest/` directory.

#### Building and Running the Example

1. **Build the project** (if you haven't already):

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

2. **Install the example** to the install directory:

```bash
cmake --install . --prefix ./install
```

3. **Run the example**:

```bash
cd install/bin
./run_example.sh
```

#### What the Example Demonstrates

The example (`examples/example-rest/main.cpp`) showcases:

- **🎵 Async Audio Upload**: Uploads a test audio file (`testing.wav`) asynchronously while performing other tasks
- **📝 Transcription Job Creation**: Creates a comprehensive transcription job with multiple features enabled
- **👥 Speaker Diarization**: Identifies different speakers in the audio
- **🛡️ Content Moderation**: Applies content moderation to the transcription
- **📱 Subtitle Generation**: Generates both VTT and SRT subtitle formats
- **📊 Sentence-level Timestamps**: Provides detailed timing information
- **🌍 Multi-language Translation**: Translates the transcription to French, German, and Spanish
- **🔄 Job Polling**: Polls the transcription status until completion
- **📋 Results Management**: Lists all transcription results and cleans them up

#### Example Features Configuration

```cpp
gladiapp::v2::request::TranscriptionRequest request;
request.audio_url = response.audio_url;
request.diarization = true;                    // Speaker identification
request.moderation = true;                     // Content moderation
request.subtitles = true;                      // Subtitle generation
request.sentences = true;                      // Sentence timestamps

// Configure subtitle formats
request.subtitles_config = gladiapp::v2::request::SubtitlesConfig();
request.subtitles_config->formats = {
    gladiapp::v2::request::SubtitlesConfig::Format::VTT,
    gladiapp::v2::request::SubtitlesConfig::Format::SRT
};

// Configure translation
request.translation = true;
request.translation_config.target_languages = {"fr", "de", "es"};
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
- **`run_example.sh`**: Launcher script that sets up the library path

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

### GladiaRestClient

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

### Configuration Options

#### TranscriptionRequest Features

- `diarization` - Speaker identification
- `translation` - Multi-language translation
- `subtitles` - Subtitle generation (SRT/VTT)
- `moderation` - Content moderation
- `sentences` - Sentence-level timestamps
- `named_entity_recognition` - Extract named entities
- `sentiment_analysis` - Analyze sentiment
- `summarization` - Generate summaries

## Project Structure

```text
gladiapp/
├── CMakeLists.txt              # Root CMake configuration
├── README.md                   # This file
├── LICENSE                     # MIT License file
├── .gitignore                  # Git ignore rules
├── .vscode/                    # VS Code configuration
├── build/                      # Build artifacts (generated)
├── install/                    # Installation directory (generated)
│   ├── bin/                   # Installed executables
│   ├── include/               # Installed headers
│   └── lib/                   # Installed libraries
├── scripts/                    # Build and utility scripts
│   └── run_example.sh.in      # Template for example runner script
├── examples/                   # Example applications
│   └── example-rest/          # REST API example
│       ├── CMakeLists.txt     # Example build configuration
│       ├── main.cpp           # Example source code
│       └── testing.wav        # Sample audio file for testing
└── gladiapp/                  # Main library source
    ├── CMakeLists.txt         # Library CMake configuration
    ├── include/               # Header files
    │   └── gladiapp/
    │       ├── gladiapp.hpp           # Main library header
    │       ├── gladiapp_rest.hpp      # REST API client header
    │       ├── utils.hpp              # Utility functions
    │       └── impl/                  # Implementation details
    │           └── gladia_rest_client_boost_impl.hpp
    └── src/                   # Implementation files
        ├── gladiapp_rest.cpp          # REST client implementation
        ├── gladiapp_rest_request.cpp  # Request structures
        └── gladiapp_rest_response.cpp # Response structures
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
  - [ ] Unit tests for all API endpoints
  - [ ] Integration tests with mock server
  - [ ] Performance benchmarks

- [ ] **Features**
  - [ ] Real-time transcription support
  - [ ] Webhook callback implementation
  - [ ] Batch processing capabilities
  - [ ] Custom vocabulary configuration
  - [ ] Audio format validation

- [ ] **Documentation**
  - [ ] Complete API documentation with Doxygen
  - [ ] More usage examples
  - [ ] Tutorial for beginners
  - [ ] Performance optimization guide

- [ ] **Error Handling**
  - [ ] Better error messages
  - [ ] Retry mechanisms
  - [ ] Rate limiting handling
  - [ ] Network timeout configuration

- [ ] **Performance**
  - [ ] Connection pooling
  - [ ] Streaming uploads for large files
  - [ ] Memory optimization
  - [ ] Async/await pattern improvements

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
