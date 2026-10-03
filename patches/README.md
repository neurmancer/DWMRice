# Patch provenance

`st-scrollback-0.9.2.diff` is the unmodified base patch downloaded from:
https://st.suckless.org/patches/scrollback/st-scrollback-0.9.2.diff

The source has already been patched; do not apply it again. Local follow-up changes bound scrolling to initialized history, isolate the alternate screen, reset history on terminal reset, and add Shift-wheel controls. See the upstream patch page for its contributor list and `suckless/st/LICENSE` for the project's license.

DWM's workspace state, gap handling, scratchpad, and Linux PID-based swallowing are integrated locally with the repository's existing Fibonacci, flycolors, and extra-bar code. They do not require applying additional upstream patches or linking XCB.
