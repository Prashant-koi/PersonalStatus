#!/bin/bash
# Build script for PersonalStatus Hyprland/Wayland version

set -e  # Exit on error

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BLUE}=================================================="
echo "Personal Status Monitor - Hyprland/Wayland Edition"
echo "Build Script"
echo -e "==================================================${NC}"
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}ERROR: CMakeLists.txt not found!${NC}"
    echo "Please run this script from the project root directory"
    exit 1
fi

# Check dependencies
echo -e "${BLUE}🔍 Checking dependencies...${NC}"

check_pkg() {
    if ! pkg-config --exists "$1" 2>/dev/null; then
        echo -e "${RED}✗ Missing: $1${NC}"
        return 1
    else
        echo -e "${GREEN}✓ Found: $1 ($(pkg-config --modversion $1))${NC}"
        return 0
    fi
}

MISSING_DEPS=0
check_pkg "wayland-client" || MISSING_DEPS=1
check_pkg "gtk4" || MISSING_DEPS=1
check_pkg "gtk-layer-shell-0" || MISSING_DEPS=1
check_pkg "cairo" || MISSING_DEPS=1
check_pkg "pango" || MISSING_DEPS=1

if [ $MISSING_DEPS -eq 1 ]; then
    echo ""
    echo -e "${RED}Missing dependencies! Run:${NC}"
    echo -e "${YELLOW}  ./scripts/install-deps-arch.sh${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}📦 Downloading Wayland protocols...${NC}"

# Download wlr-layer-shell protocol if not present
if [ ! -f "protocols/wlr-layer-shell-unstable-v1.xml" ]; then
    mkdir -p protocols
    curl -L -o protocols/wlr-layer-shell-unstable-v1.xml \
        https://raw.githubusercontent.com/swaywm/wlr-protocols/master/unstable/wlr-layer-shell-unstable-v1.xml
    echo -e "${GREEN}✓ Downloaded wlr-layer-shell protocol${NC}"
else
    echo -e "${GREEN}✓ Protocol already exists${NC}"
fi

echo ""
echo -e "${BLUE}🏗️  Configuring build with CMake...${NC}"

# Create build directory
mkdir -p build
cd build

# Run CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

echo ""
echo -e "${BLUE}🔨 Compiling...${NC}"

# Build with all CPU cores
make -j$(nproc)

echo ""
echo -e "${GREEN}✅ Build completed successfully!${NC}"
echo ""
echo -e "${YELLOW}📋 Output:${NC}"
echo -e "  Executable: ${GREEN}build/personal-status-monitor${NC}"
echo ""
echo -e "${YELLOW}🚀 To run:${NC}"
echo -e "  ${GREEN}./build/personal-status-monitor${NC}"
echo ""
echo -e "${YELLOW}📦 To install system-wide:${NC}"
echo -e "  ${GREEN}cd build && sudo make install${NC}"
echo ""