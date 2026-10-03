#!/bin/sh
# Build in a temporary directory: never reuse the repository's old binaries.
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
mode=${1:---check}
case "$mode" in --check|--install) ;; *) echo 'Usage: scripts/install.sh [--check|--install]' >&2; exit 2;; esac
for tool in make cc pkg-config tic; do
    command -v "$tool" >/dev/null 2>&1 || { echo "Missing dependency: $tool" >&2; exit 1; }
done
pkg-config --exists x11 xft xinerama fontconfig || {
    echo 'Missing X11 development libraries. See README dependencies.' >&2; exit 1;
}
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT
trap 'exit 1' HUP INT TERM
jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf 2)
# Keep some headroom on the desktop while compiling.
[ "$jobs" -le 8 ] || jobs=8
for project in dwm dmenu st; do
    cp -R "$root/suckless/$project" "$build/$project"
    make -C "$build/$project" clean
    make -C "$build/$project" -j"$jobs" CC=cc
done
if [ "$mode" = --check ]; then
    echo 'All three clean builds passed; nothing installed.'
    exit 0
fi
prefix=$HOME/.local
config=${XDG_CONFIG_HOME:-$HOME/.config}
data=${XDG_DATA_HOME:-$HOME/.local/share}
backup=$(mktemp -d "$HOME/.dwmrice-backup.XXXXXXXX")
# Every replaced file gets a numbered backup and a tab-separated restore map.
n=0
copy() {
    source=$1 target=$2 perms=$3
    if [ -e "$target" ] || [ -L "$target" ]; then
        n=$((n+1))
        cp -a "$target" "$backup/$n"
        printf '%s\t%s\n' "$n" "$target" >> "$backup/restore.tsv"
        rm -f "$target"
    else
        printf '%s\n' "$target" >> "$backup/created.txt"
    fi
    mkdir -p "$(dirname "$target")"
    install -m "$perms" "$source" "$target"
}
for project in dwm dmenu st; do
    copy "$build/$project/$project" "$prefix/bin/$project" 755
done
for file in dmenu_run dmenu_path stest; do
    copy "$build/dmenu/$file" "$prefix/bin/$file" 755
done
for file in rice-session rice-status rice-menu rice-lock rice-shot rice-wallpaper; do
    copy "$root/scripts/$file" "$prefix/bin/$file" 755
done
# Back up existing st terminfo entries before tic replaces them.
for entry in "$HOME/.terminfo/s"/st* "$HOME/.terminfo/73"/st*; do
    [ -f "$entry" ] || continue
    n=$((n+1))
    cp -a "$entry" "$backup/$n"
    printf '%s\t%s\n' "$n" "$entry" >> "$backup/restore.tsv"
done
# Explicit output directory makes this safe to test with a temporary HOME.
mkdir -p "$HOME/.terminfo"
tic -sx -o "$HOME/.terminfo" "$root/suckless/st/st.info"
copy "$root/.xinitrc" "$HOME/.xinitrc" 755
copy "$root/config/dunst/nsd.conf" "$config/dunst/nsd.conf" 644
copy "$root/assets/nsd-schematic.png" "$data/dwmrice/nsd-schematic.png" 644
copy "$root/assets/midnight-relay.png" "$data/dwmrice/midnight-relay.png" 644
copy "$root/config/kitty/midnight-relay.conf" "$config/kitty/midnight-relay.conf" 644
copy "$root/config/dwmrice/monitors.sh" "$config/dwmrice/monitors.sh" 755
copy "$root/config/dwmrice/monitors.sh.example" "$config/dwmrice/monitors.sh.example" 644
printf '\nInstalled Midnight Relay. Backups: %s\nStart from a TTY with startx.\n' "$backup"
