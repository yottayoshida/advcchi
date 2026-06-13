# Clawd Sprite Design Notes

Reference date: 2026-06-13
Source: Claude Code CLI terminal display (local observation)

## Color palette (RGB888 → RGB565)

| Index | Name       | RGB888    | RGB565   | Usage                      |
|-------|------------|-----------|----------|----------------------------|
| 0     | Transparent| #000000   | 0x0000   | Background (black)         |
| 1     | Body       | #4A90D9   | 0x4C9B   | Main body blue             |
| 2     | Dark       | #2D5F9E   | 0x2AF3   | Outline, shadow, legs      |
| 3     | Light      | #7CB3E8   | 0x7D9D   | Body highlight             |
| 4     | Claw       | #F0A030   | 0xF506   | Claw/pincer (orange accent)|
| 5     | Claw Dark  | #C07820   | 0xC3C4   | Claw shadow (reserved)     |
| 6     | Eye White  | #FFFFFF   | 0xFFFF   | Eye sclera                 |
| 7     | Eye Pupil  | #1A1A2E   | 0x18C5   | Eye pupil                  |
| 8     | Cheek      | #FF9999   | 0xFCD3   | Blush marks                |
| 9     | Belly      | #8BC4F0   | 0x8E3E   | Belly lighter area         |

## Sprite spec

- Resolution: 16x16 pixels
- Display scale: 5x → 80x80 px on 240x135 display
- Format: indexed color (1 byte per pixel, palette lookup)
- Storage: PROGMEM (flash, not RAM)
- Expressions: IDLE, BLINK, HAPPY, SURPRISED, SLEEPY (256 bytes each)

## Design decisions

- Blue body with orange claws: matches Claude Code's current aesthetic
- 16x16 resolution: classic tamagotchi feel, compact storage, easy to iterate
- Full sprite per expression (not overlays): simpler code, 1.2KB total is negligible
- Color palette is defined in one location (clawd_sprites.h) for easy updates
- Scale factor as constant for easy adjustment after hardware testing

## Rights note

No official Claude Code assets are redistributed. These sprites are original
pixel art created with Claude Code's character as visual reference. For personal
hobby use only. Reference screenshots are local-only (not committed to git).
