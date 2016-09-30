# aaaa-gcw0
## Progress:
- Controls
- Graphics code fixes
- Save file handling
- Haptic feedback

## Controls:
- A: Confirm/Sword
- B: Jump
- X: Shout/Confirm Quit
- Y: Fire Gun
- L: Aim gun
- R: Look
- Start Enter/Menu
- Select Esc/Torch
- DPAD: Movement
- Joypad: Analogue movement
- HOLD: Exit to OS immediately

## Known bugs:
- Force feedback is glitchy due to GCW0's driver (needs firmware fix)

## TODO:
- G-Sensor support
- OPK instruction manual

## Note:
- PCLINUX build requires libshake; please build and install it.
- [GET LIBSHAKE HERE](https://github.com/zear/libShake)

## Build notes:
- "env PLATFORM=PC make" to build PCLINUX test build
- "make opk" to build GCW opk build
