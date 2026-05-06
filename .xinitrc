#!bin/sh
#!/bin/sh

# Dual-monitor setup - HDMI-A-1 (left) + DisplayPort-0 (right, primary)
xrandr --output DisplayPort-0 --primary --mode 1920x1080 \
       --output HDMI-A-1 --mode 1920x1080 --left-of DisplayPort-0

pulseaudio --start &

exec dwm
