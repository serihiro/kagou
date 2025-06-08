#!/usr/bin/env python3
import socket
import time
import re

def read_http_response(sock):
    """Read a complete HTTP response including headers and body"""
    # Read headers
    headers = b""
    while b"\r\n\r\n" not in headers:
        chunk = sock.recv(1)
        if not chunk:
            break
        headers += chunk
    
    headers_str = headers.decode('utf-8', errors='ignore')
    
    # Extract Content-Length
    content_length = 0
    match = re.search(r'Content-length:\s*(\d+)', headers_str, re.IGNORECASE)
    if match:
        content_length = int(match.group(1))
    
    # Read body
    body = b""
    while len(body) < content_length:
        chunk = sock.recv(content_length - len(body))
        if not chunk:
            break
        body += chunk
    
    return headers_str + body.decode('utf-8', errors='ignore')

def test_keepalive():
    # Create a socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        # Connect to the server
        sock.connect(('localhost', 8080))
        print("Connected to server")
        
        # Test 1: Send HTTP/1.1 request (should keep connection alive by default)
        print("\n=== First Request (HTTP/1.1) ===")
        request1 = b"GET /test/test.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
        sock.send(request1)
        response1 = read_http_response(sock)
        print("Headers:")
        print(response1.split('\r\n\r\n')[0])
        
        # Check if connection is still alive by sending another request
        print("\n=== Second Request (same connection) ===")
        request2 = b"GET /test/test.css HTTP/1.1\r\nHost: localhost\r\n\r\n"
        sock.send(request2)
        response2 = read_http_response(sock)
        print("Headers:")
        print(response2.split('\r\n\r\n')[0])
        
        # Test 2: Send request with Connection: close
        print("\n=== Third Request (Connection: close) ===")
        request3 = b"GET /test/test.js HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
        sock.send(request3)
        response3 = read_http_response(sock)
        print("Headers:")
        print(response3.split('\r\n\r\n')[0])
        
        # Try to send another request (should fail)
        print("\n=== Fourth Request (should fail) ===")
        time.sleep(0.5)
        try:
            request4 = b"GET /test/test.json HTTP/1.1\r\nHost: localhost\r\n\r\n"
            sock.send(request4)
            # Try to read response
            sock.settimeout(1.0)  # Set timeout to avoid hanging
            response4 = sock.recv(1)
            if response4:
                print("ERROR: Connection should have been closed!")
            else:
                print("Connection closed as expected")
        except (socket.timeout, ConnectionResetError, BrokenPipeError):
            print("Connection closed as expected")
            
    except Exception as e:
        print(f"Error: {e}")
    finally:
        try:
            sock.close()
        except:
            pass

def test_http10():
    # Test HTTP/1.0 (should close connection by default)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        sock.connect(('localhost', 8080))
        print("\n\n=== HTTP/1.0 Test ===")
        
        request = b"GET /test/test.html HTTP/1.0\r\n\r\n"
        sock.send(request)
        response = read_http_response(sock)
        print("Headers:")
        print(response.split('\r\n\r\n')[0])
        
        # Try to send another request (should fail)
        print("\n=== Second Request (should fail for HTTP/1.0) ===")
        time.sleep(0.5)
        try:
            request2 = b"GET /test/test.css HTTP/1.0\r\n\r\n"
            sock.send(request2)
            sock.settimeout(1.0)
            response2 = sock.recv(1)
            if response2:
                print("ERROR: Connection should have been closed for HTTP/1.0!")
            else:
                print("Connection closed as expected for HTTP/1.0")
        except (socket.timeout, ConnectionResetError, BrokenPipeError):
            print("Connection closed as expected for HTTP/1.0")
            
    except Exception as e:
        print(f"Error: {e}")
    finally:
        try:
            sock.close()
        except:
            pass

if __name__ == "__main__":
    print("Testing HTTP Keep-Alive functionality...")
    print("Make sure the server is running on port 8080")
    print("=" * 50)
    
    test_keepalive()
    test_http10()
