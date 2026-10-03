#!/bin/sh
# HDMI left, DisplayPort right and primary. Resolve names in this Xorg session:
# AMD's native Xorg names can differ from Xwayland's DP-2 / HDMI-A-2.
set -eu
command -v xrandr >/dev/null 2>&1 || exit 0
outputs=$(xrandr --query) || exit 1
dp=$(printf '%s\n' "$outputs" | awk '$2 == "connected" && $1 ~ /^(DisplayPort-|DP-)/ {print $1; exit}')
hdmi=$(printf '%s\n' "$outputs" | awk '$2 == "connected" && $1 ~ /^HDMI/ {print $1; exit}')
# A disconnected display should never prevent the session from starting.
[ -n "$dp" ] && [ -n "$hdmi" ] || exit 0
supports_1080p() {
    printf '%s\n' "$outputs" | awk -v output="$1" '
        /^[^[:space:]]/ {selected=($1 == output)}
        selected && $1 == "1920x1080" {found=1}
        END {exit !found}'
}
# Keep the server's preferred refresh rate; do not guess a hardware refresh cap.
set -- --output "$dp" --primary
if supports_1080p "$dp"; then set -- "$@" --mode 1920x1080; else set -- "$@" --auto; fi
set -- "$@" --output "$hdmi"
if supports_1080p "$hdmi"; then set -- "$@" --mode 1920x1080; else set -- "$@" --auto; fi
exec xrandr "$@" --left-of "$dp"
