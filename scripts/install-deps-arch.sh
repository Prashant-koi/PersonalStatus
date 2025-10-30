#!/bin/bash
# Arch Linux dependency installer for PersonalStatus Hyprland/Wayland version
# Run this before building: chmod +x scripts/install-deps-arch.sh && ./scripts/install-deps-arch.sh

set -e  # Exit on error

echo "=================================================="
echo "Personal Status Monitor - Hyprland/Wayland Edition"
echo "Arch Linux Dependency Installer"
echo "=================================================="
echo ""

# Color codes for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}📦 Installing build tools...${NC}"
sudo pacman -S --needed \
    base-devel \
    cmake \
    git \
    pkg-config

echo ""
echo -e "${BLUE}🪟 Installing Wayland development libraries...${NC}"
sudo pacman -S --needed \
    wayland \
    wayland-protocols \
    wlroots \
    gtk4 \
    gtk-layer-shell

echo ""
echo -e "${BLUE}🔔 Installing system tray libraries (StatusNotifierItem)...${NC}"
sudo pacman -S --needed \
    libdbusmenu-gtk3 \
    libappindicator-gtk3

echo ""
echo -e "${BLUE}🔧 Installing additional dependencies...${NC}"
sudo pacman -S --needed \
    cairo \
    pango \
    glib2 \
    json-c \
    libnotify

echo ""
echo -e "${BLUE}🎨 Installing optional but recommended packages...${NC}"
sudo pacman -S --needed \
    ttf-dejavu \
    ttf-liberation \
    noto-fonts

echo ""
echo -e "${GREEN}✅ All dependencies installed successfully!${NC}"
echo ""
echo -e "${YELLOW}📋 Installed components:${NC}"
echo "  ✓ Build tools (gcc, cmake, pkg-config)"
echo "  ✓ Wayland core (wayland, wayland-protocols)"
echo "  ✓ GTK4 (for dialogs and rendering)"
echo "  ✓ gtk-layer-shell (for overlay positioning)"
echo "  ✓ DBus/StatusNotifierItem (for system tray)"
echo "  ✓ Utilities (cairo, pango, json-c, libnotify)"
echo ""
echo -e "${GREEN}🚀 Next steps:${NC}"
echo "  1. Generate Wayland protocols: ./scripts/generate-protocols.sh"
echo "  2. Build the project: ./scripts/build.sh"
echo ""