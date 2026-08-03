#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(dirname "$SCRIPT_DIR")"

echo "=================================================="
echo "🛠️  Compiling Native C++ Unit Tests with g++..."
echo "=================================================="

g++ -std=c++17 -Wall -I"$SCRIPT_DIR" -I"$SCRIPT_DIR/mocks" "$SCRIPT_DIR/test_wifi_dashboard.cpp" -o "$WORKSPACE_DIR/build/test_wifi_dashboard"
g++ -std=c++17 -Wall -I"$SCRIPT_DIR" -I"$SCRIPT_DIR/mocks" "$SCRIPT_DIR/test_servo_control.cpp" -o "$WORKSPACE_DIR/build/test_servo_control"

echo "=================================================="
echo "🚀 Running Tests 100% Offline / Locally..."
echo "=================================================="

"$WORKSPACE_DIR/build/test_wifi_dashboard"
"$WORKSPACE_DIR/build/test_servo_control"

