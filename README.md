# Kagou

[Kagou](https://ja.wikipedia.org/wiki/%E3%82%AB%E5%8F%B7%E8%A6%B3%E6%B8%AC%E6%A9%9F) is a lightweight HTTP server implemented in C that serves static files with HTTPS/TLS 1.2+ support. It's designed for simplicity and educational purposes, featuring a modular architecture with clear separation of concerns.

## Quick Start

```bash
$ git clone git@github.com:serihiro/kagou.git
$ cd kagou
$ make
$ ./kagou
```

## Usage

### Basic Usage

```bash
# Default usage (port 8080, current directory)
./kagou

# Custom port and directory
./kagou 5000 html

# Enable HTTPS with TLS 1.2+
./kagou 8443 html --https
```

### Build Commands

```bash
# Build the main executable
make

# Clean build
make clean && make

# Format code using clang-format
make format
```

### Testing

#### C Unit Tests
```bash
# Run all C unit tests
make test

# Run a specific test (e.g., test_request_validation)
make test-test_request_validation
```

#### E2E Tests
Install JavaScript dependencies and browsers using **yarn**:

```bash
$ yarn install
$ yarn playwright install
```

Run the Playwright tests:

```bash
$ yarn e2e
```

## Architecture

### Core Components

- **main.c**: Entry point, signal handling, SSL/TLS setup, and main server loop
- **request_handler.c/h**: Central request processing, file serving, and MIME type handling
- **request.c/h**: HTTP request parsing and validation
- **response.c/h**: HTTP response generation and formatting
- **util.c/h**: Utility functions for string manipulation and time formatting

### Key Features

- HTTP/1.1 protocol support with basic error handling (200, 404, 415, 500)
- HTTPS/TLS 1.2+ support using OpenSSL
- Static file serving with automatic MIME type detection
- Memory safety using AddressSanitizer (`-fsanitize=address`)
- Signal handling for graceful shutdown (SIGTERM, SIGINT)

## Supported Media Types

- HTML (.html, .htm)
- JavaScript (.js)
- CSS (.css)
- CSV (.csv)
- JSON (.json)
- Images: JPEG (.jpg, .jpeg), PNG (.png), GIF (.gif)

## HTTP Status Codes

- ✅ 200 OK
- ✅ 404 Not Found
- ✅ 415 Unsupported Media Type
- ✅ 500 Internal Server Error

## Security

- Uses TLS 1.2+ only (TLS 1.0/1.1 disabled for security)
- SSL certificates expected in `cert/` directory (server.crt, server.key)
- Input validation for file paths and HTTP requests
- Memory leak prevention with proper cleanup

## Development

### Coding Standards

- Function names: `snake_case`
- Struct names: `PascalCase` 
- Macros: `UPPER_SNAKE_CASE`
- Indentation: 2 spaces
- All error conditions must be handled
- Use `extern` for shared variables in headers (not `static`)

### Testing Structure

- **tests/**: C unit tests using basic assert macros
- **e2e/**: Playwright-based integration tests
- **test/**: Static test files (HTML, CSS, JS, images) for manual testing

## License

MIT
