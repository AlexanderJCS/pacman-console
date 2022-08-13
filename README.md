# pacman-console

A PacMan console game made in C++.

**This project is currently a work in progress.**

## Compatibility

This project is only compatible with the Windows OS and can only be ran in consoles that support ANSI escape codes.

Up to now, this project has only been compiled with C++11 in Visual Studio. If you have any compilation errors, try compiling in Visual Studio first.

## Screenshots
![image](https://user-images.githubusercontent.com/98898166/183758714-ceeec88e-f990-4937-a294-409b72c4500a.png)
![image](https://user-images.githubusercontent.com/98898166/183759052-5a02e080-0491-4c30-b749-ae16d7324f2e.png)

## Controls

Use `W`, `A`, `S`, and `D` to move. Collect all the pellets to win.

## Compiling

You can compile this program using the following command: `g++ ./pacMan.cpp ./levelUtils.cpp ./gameObject.cpp`

## Planned features

- Configuration files for refresh rate and ghost movement speed
- Power pellets (puts ghosts in a blue, frightened mode)
- PacMan faces the direction he is moving
- Using conio.h for input instead of windows.h so the player does not need to hold the movement key down until there is a new frame.

## Known issues
Ghost movement may stutter because they only update 2 out of 3 frames. This is to make the game easier. In the future, there will be a configuration file to allow the user to modify ghost speed.

- Sometimes PacMan can teleport through a ghost if Pacman and the ghost are moving head on.
