# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Kagou is a lightweight HTTP server implemented in C that serves static files with HTTPS/TLS 1.2+ support. It's designed for simplicity and educational purposes, featuring a modular architecture with clear separation of concerns.

## Build and Development Commands

### Building
```bash
# Build the main executable
make

# Clean build
make clean && make

# Format code using clang-format
make format
```

### Testing
```bash
# Run all C unit tests
make test

# Run a specific test (e.g., test_request_validation)
make test-test_request_validation

# Run E2E tests with Playwright
yarn install          # Install dependencies first
yarn playwright install  # Install browsers
yarn e2e              # Run E2E tests
```

### Running the Server
```bash
# Default usage (port 8080, current directory)
./kagou

# Custom port and directory
./kagou 5000 html

# Enable HTTPS with TLS 1.2+
./kagou 8443 html --https
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

### Data Structures

- `http_response`: Response structure with status, headers, and body
- `mime_map`: Extension to MIME type mapping
- `KEY_VALUE`/`Tuple`: Generic key-value pair structures

### Security Considerations

- Uses TLS 1.2+ only (TLS 1.0/1.1 disabled for security)
- SSL certificates expected in `cert/` directory (server.crt, server.key)
- Input validation for file paths and HTTP requests
- Memory leak prevention with proper cleanup

## Coding Standards

- Function names: `snake_case`
- Struct names: `PascalCase` 
- Macros: `UPPER_SNAKE_CASE`
- Indentation: 2 spaces
- All error conditions must be handled
- Use `extern` for shared variables in headers (not `static`)

## Testing Structure

- **tests/**: C unit tests using basic assert macros
- **e2e/**: Playwright-based integration tests
- **test/**: Static test files (HTML, CSS, JS, images) for manual testing

Each test executable is built individually and can be run via `make test-<testname>`.