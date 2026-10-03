#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
for script in "$root"/scripts/* "$root/.xinitrc"; do
    sh -n "$script"
done
snapshot=$("$root/scripts/rice-status" --once)
case "$snapshot" in *'; NSD / CRASH HARDWARE'*'RAM '*) ;; *) echo 'Invalid status output' >&2; exit 1;; esac
stage=$(mktemp -d)
trap 'rm -rf "$stage"' EXIT
trap 'exit 1' HUP INT TERM
mkdir -p "$stage/home"
printf 'existing session\n' > "$stage/home/.xinitrc"
env HOME="$stage/home" XDG_CONFIG_HOME="$stage/config" XDG_DATA_HOME="$stage/data" \
    "$root/scripts/install.sh" --install > "$stage/build.log" 2>&1 || { cat "$stage/build.log"; exit 1; }
for binary in dwm dmenu st stest dmenu_run dmenu_path rice-session rice-status rice-menu rice-lock rice-shot rice-wallpaper; do
    test -x "$stage/home/.local/bin/$binary"
done
test -s "$stage/data/dwmrice/midnight-relay.png"
test -s "$stage/data/dwmrice/nsd-schematic.png"
test -s "$stage/config/dunst/nsd.conf"
test -s "$stage/config/kitty/midnight-relay.conf"
cmp "$root/.xinitrc" "$stage/home/.xinitrc"
backup=$(find "$stage/home" -maxdepth 1 -type d -name '.dwmrice-backup.*')
number=$(awk -F '\t' -v target="$stage/home/.xinitrc" '$2 == target {print $1}' "$backup/restore.tsv")
test "$(cat "$backup/$number")" = 'existing session'
env TERMINFO="$stage/home/.terminfo" infocmp st-256color >/dev/null
printf 'PASS: shell syntax, telemetry, clean builds, staged install, backups, terminfo.\n'
