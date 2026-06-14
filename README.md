# clawd-cardputer

A developer tamagotchi running on M5Stack Cardputer ADV. It reacts to your Claude Code activity via hooks — every tool call, subagent spawn, permission prompt, and session end feeds the creature on your desk.

## What it does

A pixel-art character ("Clawd") lives on the Cardputer's 240x135 display. It walks, bobs, breathes, and blinks on its own. When Claude Code fires hook events, the hook script sends them over USB serial, and Clawd reacts:

| Event | Expression | Sound | Source |
|-------|-----------|-------|--------|
| `bash` `read` `glob` `grep` `edit` | Blink | — | tool name |
| `write` `search` | Happy | tone | tool name |
| `test_pass` `clean` | Happy | tone | command parse |
| `test_fail` `conflict` | Surprised | tone | command parse |
| `branch` `pr_open` | Excited | tone | command parse |
| `dirty` | Sleepy | tone | command parse |
| `commit` `push` | Excited | tone | tool name |
| `tool_fail` | Disappointed | low tone | PostToolUseFailure |
| `stop` | Happy | tone | Stop (session end) |
| `stop_fail` | Panicking | low tone | StopFailure |
| `perm_ask` | Confused + "???" | short tone | PermissionRequest |
| `subagent_start` | Happy + mini spawn | fanfare | SubagentStart |
| `subagent_stop` | *(linger)* | tone | SubagentStop |
| `/develop` | **Party mode** | melody | skill |

The hook parses `tool_input.command` for Bash invocations to detect git operations (`git status`, `git checkout -b`, `gh pr create`, merge conflicts) and test runners (`pytest`, `cargo test`, `npm test`, and common wrappers like `uv run pytest`).

### Mini Clawds (subagent companions)

When Claude Code spawns subagents, up to 2 smaller Clawds appear on the left and right edges of the display. They walk and bob independently. Minis linger for 8 seconds after the last subagent event, then auto-despawn. They clear automatically when the parent enters sleep.

### Sleep cycle

After 1 minute of inactivity, Clawd falls asleep — it squishes flat, breathes slowly, and "zzz" animates above it (z → zz → zzz). Any event or key press wakes it up with a surprised reaction.

Keyboard shortcuts on the Cardputer itself:

| Key | Action |
|-----|--------|
| `p` | Party mode |
| `m` | Mute / unmute |
| `1` `2` `3` | Volume level |
| `Enter` | Pet (excited + sound) |
| Any other | Random reaction |

## Hardware

- [M5Stack Cardputer ADV](https://shop.m5stack.com/products/m5stack-cardputer-adv) (ESP32-S3, 240x135 display, keyboard, speaker)
- USB-C cable (for flashing and serial communication)

## Setup

### 1. Flash the firmware

Requires [PlatformIO](https://platformio.org/).

```bash
cd firmware
pio run -t upload
```

> If you're behind a corporate proxy with custom CA certificates:
> ```bash
> REQUESTS_CA_BUNDLE=$HOME/.your-ca.pem pio run -t upload
> ```

### 2. Install the Claude Code hook

```bash
./hook/install-hook.sh
```

This registers hooks for 7 event types (PostToolUse, SubagentStart, SubagentStop, PostToolUseFailure, Stop, StopFailure, PermissionRequest) in `~/.claude/settings.json`. The script is safe to re-run (idempotent) and creates a backup before modifying settings.

Options:
- `status` — show which hooks are installed
- `--dry-run` — show what would change without modifying anything
- `uninstall` — remove all hook entries

### 3. Restart Claude Code

The hook takes effect on the next Claude Code session.

## Requirements

- Python 3 with [pyserial](https://pypi.org/project/pyserial/) (`pip install pyserial`)
- [jq](https://jqlang.github.io/jq/) (for JSON parsing in the hook script)
- macOS (tested on macOS 15 Sequoia; Linux should work but is untested)

## Project structure

```
firmware/
  platformio.ini        # PlatformIO build config
  src/main.cpp          # Application logic (animations, events, serial, party mode)
  src/clawd_sprites.h   # Pixel-art character data (16x10 grid, 10 expressions)
  assets/source/        # Design notes (not redistributed)
hook/
  clawd-hook.sh         # Hook script (classifies events, sends over serial)
  install-hook.sh       # Hook installer (manages 7 event types in settings.json)
```

## How the hook works

1. Claude Code fires hook events (PostToolUse, SubagentStart, SubagentStop, PostToolUseFailure, Stop, StopFailure, PermissionRequest)
2. `clawd-hook.sh` reads the JSON event from stdin (PostToolUse) or receives the event name as `$1` (all others)
3. For Bash tool calls, it parses `tool_input.command` to classify git operations and test results; otherwise falls back to `tool_name`
4. A single fixed-string event name is sent over USB serial (115200 baud) via pyserial
5. The Cardputer's firmware matches it against the event table and triggers the appropriate reaction

The hook is non-blocking — if the Cardputer is disconnected, it silently does nothing.

## Configuration

| Variable | Default | Description |
|----------|---------|-------------|
| `CLAWD_PORT` | `/dev/cu.usbmodem*` (first match) | Serial port override |

## License

[MIT](LICENSE)
