# Lander

This is a lunar lander style game for Linux and Windows. Use the arrow
keys to move the ship and avoid crashing into obstacles. Collect all the
spinning keys and then land the ship on one of the landing pads. The
levels are randomly generated and become progressively harder.

# Controls

| Key          | Action              |
|--------------|---------------------|
| Left arrow   | Turn left           |
| Right arrow  | Turn right          |
| Up arrow     | Thrust              |
| P            | Pause               |
| PrintScreen  | Take a screenshot   |
| Esc          | Quit / exit to menu |
| Enter        | Select option       |

You can also use a game pad, if you have one.

# Installation

## From source

Lander uses the [Meson](https://mesonbuild.com/) build system. 

    $ mkdir build && cd build
    $ meson ..
    $ ninja install

# About

Developed by Nick Gasson <nick@nickg.me.uk>.

This code is distributed under the GNU General Public license. See the
file COPYING for more details.
