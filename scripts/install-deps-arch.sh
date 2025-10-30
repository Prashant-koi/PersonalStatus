#!/bin/bash
set -e

echo "Installing dependencies for Hyprland/Wayland..."

sudo pacman -S --needed \
    base-devel \
    cmake \
    git \
    pkg-config \
    wayland \
    wayland-protocols \
    gtk4 \
    gtk4-layer-shell \
    libcurl-gnutls \
    cairo \
    pango \
    glib2 \
    json-c

echo "✅ All dependencies installed!"
echo ""
echo "Next steps:"
echo "  1. chmod +x scripts/build.sh"
echo "  2. ./scripts/build.sh"