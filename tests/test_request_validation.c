#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/request.h"

void test_http_09_request() {
    printf("Testing HTTP/0.9 request (no headers)...\n");
    
    char *raw_request = "GET /index.html\r\n\r\n";
    Request *req = Request_new(raw_request);
    
    // Check request line
    assert(strcmp(req->request_header_values[0].key, "method") == 0);
    assert(strcmp(req->request_header_values[0].value, "GET") == 0);
    assert(strcmp(req->request_header_values[1].key, "path") == 0);
    assert(strcmp(req->request_header_values[1].value, "/index.html") == 0);
    assert(strcmp(req->request_header_values[2].key, "http_version") == 0);
    assert(strcmp(req->request_header_values[2].value, "HTTP/0.9") == 0);
    
    // HTTP/0.9 should not have any headers
    assert(strlen(req->request_header_values[3].key) == 0);
    
    Request_delete(req);
    printf("HTTP/0.9 test passed!\n\n");
}

void test_http_10_request() {
    printf("Testing HTTP/1.0 request with valid and invalid headers...\n");
    
    char *raw_request = "GET /index.html HTTP/1.0\r\n"
                       "Host: example.com\r\n"
                       "User-Agent: TestClient/1.0\r\n"
                       "Invalid-Header-Name!: should be ignored\r\n"
                       "Transfer-Encoding: chunked\r\n"  // HTTP/1.1 only header
                       "Connection: keep-alive\r\n"
                       "\r\n";
    
    Request *req = Request_new(raw_request);
    
    // Check request line
    assert(strcmp(req->request_header_values[0].key, "method") == 0);
    assert(strcmp(req->request_header_values[0].value, "GET") == 0);
    assert(strcmp(req->request_header_values[1].key, "path") == 0);
    assert(strcmp(req->request_header_values[1].value, "/index.html") == 0);
    assert(strcmp(req->request_header_values[2].key, "http_version") == 0);
    assert(strcmp(req->request_header_values[2].value, "HTTP/1.0") == 0);
    
    // Check valid headers
    assert(strcmp(req->request_header_values[3].key, "Host") == 0);
    assert(strcmp(req->request_header_values[3].value, "example.com") == 0);
    assert(strcmp(req->request_header_values[4].key, "User-Agent") == 0);
    assert(strcmp(req->request_header_values[4].value, "TestClient/1.0") == 0);
    assert(strcmp(req->request_header_values[5].key, "Connection") == 0);
    assert(strcmp(req->request_header_values[5].value, "keep-alive") == 0);
    
    // Invalid headers should be skipped
    // Transfer-Encoding is HTTP/1.1 only, should not be present
    for (int i = 3; i < REQUEST_HEADER_ITEM_MAX_SIZE; i++) {
        if (strlen(req->request_header_values[i].key) > 0) {
            assert(strcmp(req->request_header_values[i].key, "Transfer-Encoding") != 0);
            assert(strstr(req->request_header_values[i].key, "!") == NULL);
        }
    }
    
    // Check keep-alive setting
    assert(req->keep_alive == 1);  // Explicitly set to keep-alive
    
    Request_delete(req);
    printf("HTTP/1.0 test passed!\n\n");
}

void test_http_11_request() {
    printf("Testing HTTP/1.1 request with various headers...\n");
    
    char *raw_request = "POST /api/data HTTP/1.1\r\n"
                       "Host: api.example.com\r\n"
                       "Content-Type: application/json\r\n"
                       "Content-Length: 42\r\n"
                       "Transfer-Encoding: chunked\r\n"
                       "Connection: close\r\n"
                       "Invalid@Header: should be ignored\r\n"
                       "\r\n";
    
    Request *req = Request_new(raw_request);
    
    // Check request line
    assert(strcmp(req->request_header_values[0].key, "method") == 0);
    assert(strcmp(req->request_header_values[0].value, "POST") == 0);
    assert(strcmp(req->request_header_values[1].key, "path") == 0);
    assert(strcmp(req->request_header_values[1].value, "/api/data") == 0);
    assert(strcmp(req->request_header_values[2].key, "http_version") == 0);
    assert(strcmp(req->request_header_values[2].value, "HTTP/1.1") == 0);
    
    // Check valid headers
    int header_count = 0;
    for (int i = 3; i < REQUEST_HEADER_ITEM_MAX_SIZE; i++) {
        if (strlen(req->request_header_values[i].key) > 0) {
            header_count++;
            // Invalid header should not be present
            assert(strstr(req->request_header_values[i].key, "@") == NULL);
        }
    }
    
    // Should have 5 valid headers (Host, Content-Type, Content-Length, Transfer-Encoding, Connection)
    assert(header_count == 5);
    
    // Check keep-alive setting (should be 0 due to Connection: close)
    assert(req->keep_alive == 0);
    
    Request_delete(req);
    printf("HTTP/1.1 test passed!\n\n");
}

void test_malformed_request() {
    printf("Testing malformed request...\n");
    
    char *raw_request = "GET\r\n\r\n";  // Missing path and version
    Request *req = Request_new(raw_request);
    
    // Should handle gracefully
    assert(strcmp(req->request_header_values[0].key, "method") == 0);
    assert(strcmp(req->request_header_values[0].value, "GET") == 0);
    
    Request_delete(req);
    printf("Malformed request test passed!\n\n");
}

int main() {
    printf("Running request validation tests...\n\n");
    
    test_http_09_request();
    test_http_10_request();
    test_http_11_request();
    test_malformed_request();
    
    printf("All tests passed!\n");
    return 0;
}
