#!/bin/sh
# Preserve Arch's system X initialization (including environment integration).
if [ -d /etc/X11/xinit/xinitrc.d ]; then
    for hook in /etc/X11/xinit/xinitrc.d/?*.sh; do
        [ ! -x "$hook" ] || . "$hook"
    done
    unset hook
fi
export PATH="$HOME/.local/bin:$PATH"
exec rice-session
