# aaaa-gcw0
## Progress:
- Controls
- Graphics code fixes
- Save file handling
- Haptic feedback
- G-Sensor

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
- POWER: Re-centre G-Sensor

## Known bugs:
- Force feedback is glitchy due to GCW0's driver (needs firmware fix)
- G-Sensor centre function needs improvement

## TODO:
- OPK instruction manual

## Note:
- PCLINUX build requires libshake; please build and install it.
- [GET LIBSHAKE HERE](https://github.com/zear/libShake)
- PCLINUX build supports gamepad analogue, specify 4 axes on joy0 by argv. Second pair emulates G-Sensor; Home key to re-centre.

Example: (for joystick with axes x, y, rx, ry at the corresponding addresses)

cd ./opk_build && ./aaaa 0 1 2 3

## Build notes:
- "env PLATFORM=PC make" to build PCLINUX test build
- "make opk" to build GCW opk build
