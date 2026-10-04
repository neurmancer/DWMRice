#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
stage=$(mktemp -d)
trap 'rm -rf "$stage"' EXIT
trap 'exit 1' HUP INT TERM
mkdir -p "$stage/bin"
export XDG_STATE_HOME="$stage/state" MEDIA_FIXTURE="$stage/metadata" MEDIA_LOG="$stage/log"
export PATH="$stage/bin:$PATH"
cat > "$stage/bin/playerctl" <<'MOCK'
#!/bin/sh
printf '%s\n' "$*" >> "$MEDIA_LOG"
[ "$1" = -p ] || exit 9
case "$2" in spotify|ShittyJukeBox) ;; *) exit 9;; esac
case "$3" in
    metadata) [ -f "$MEDIA_FIXTURE" ] || exit 1; cat "$MEDIA_FIXTURE" ;;
    status) [ ! -f "$MEDIA_FIXTURE.offline" ] || exit 1; printf 'Stopped\n' ;;
    *) exit 9 ;;
esac
MOCK
chmod +x "$stage/bin/playerctl"
printf 'Playing // Artist - Song\n' > "$MEDIA_FIXTURE"
[ "$("$root/scripts/rice-media")" = 'SPOTIFY // Playing // Artist - Song' ]
"$root/scripts/rice-media" --toggle
[ "$(cat "$XDG_STATE_HOME/dwmrice/media-player")" = jukebox ]
printf 'Paused // Björk;\nTitle|test\033\tend\n' > "$MEDIA_FIXTURE"
result=$("$root/scripts/rice-media")
case "$result" in 'SHITTYJUKEBOX // Paused // Björk '*Title*test*end*) ;; *) exit 1;; esac
case "$result" in *';'*|*'|'*) exit 1;; esac
"$root/scripts/rice-media" --toggle
[ "$(cat "$XDG_STATE_HOME/dwmrice/media-player")" = spotify ]
rm "$MEDIA_FIXTURE"
[ "$("$root/scripts/rice-media")" = 'SPOTIFY // Stopped // NO TRACK' ]
touch "$MEDIA_FIXTURE.offline"
[ "$("$root/scripts/rice-media")" = 'SPOTIFY // OFFLINE' ]
awk 'BEGIN {for(i=0;i<300;i++) printf "é"}' > "$MEDIA_FIXTURE"
"$root/scripts/rice-media" > "$stage/result"
iconv -f UTF-8 -t UTF-8 "$stage/result" >/dev/null
[ "$(wc -c < "$stage/result")" -le 133 ]
# Concurrent toggles must not lose an update.
"$root/scripts/rice-media" --toggle &
a=$!
"$root/scripts/rice-media" --toggle &
b=$!
wait "$a"; wait "$b"
[ "$(cat "$XDG_STATE_HOME/dwmrice/media-player")" = spotify ]
awk '$3 != "metadata" && $3 != "status" {exit 1}' "$MEDIA_LOG"
echo 'PASS: music source selection, persistence, concurrent switches, paused/offline states, metadata sanitization.'
