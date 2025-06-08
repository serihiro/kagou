#!/bin/bash

echo "=== SIGTERM Signal Test for kagou server ==="

# サーバーを起動
echo "Starting server on port 8080..."
./kagou 8080 test &
SERVER_PID=$!
sleep 1

# プロセスが起動したか確認
if ps -p $SERVER_PID > /dev/null; then
    echo "Server started successfully (PID: $SERVER_PID)"
else
    echo "Failed to start server"
    exit 1
fi

# ポートが使用されているか確認
echo "Checking if port 8080 is in use..."
if lsof -i :8080 > /dev/null 2>&1; then
    echo "Port 8080 is in use - OK"
else
    echo "Warning: Could not verify port usage (may need sudo)"
fi

# SIGTERMを送信
echo "Sending SIGTERM signal..."
kill -TERM $SERVER_PID

# クリーンアップメッセージを待つ
sleep 1

# プロセスが終了したか確認
if ps -p $SERVER_PID > /dev/null 2>&1; then
    echo "ERROR: Server process still running after SIGTERM"
    kill -9 $SERVER_PID
    exit 1
else
    echo "Server process terminated successfully"
fi

# ポートが解放されたか確認（再度サーバーを起動してみる）
echo "Testing if port is released by starting server again..."
./kagou 8080 test &
NEW_SERVER_PID=$!
sleep 1

if ps -p $NEW_SERVER_PID > /dev/null; then
    echo "Port was successfully released - new server started (PID: $NEW_SERVER_PID)"
    # クリーンアップ
    kill -TERM $NEW_SERVER_PID
    sleep 1
    echo "Test completed successfully!"
else
    echo "ERROR: Could not start new server - port may not have been released properly"
    exit 1
fi
