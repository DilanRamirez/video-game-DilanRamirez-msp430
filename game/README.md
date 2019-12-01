# Project 3: LCD Game

## Introduction
Continuing your effort to help ACME Solutions provide Christmas toys, the
task is now to develop a game for the LCD screen. By utilizing the
knowledge havegained working with the buttons and buzzer. In addition, a
series of libraries have been found to assist you in creating your game.

## Requirements
The game should meet the following minimum criteria:

    1. Dynamically render graphical elements that move and change -- including one
       original algorithmically rendered graphical element.
    2. Handle colisions properly with other shapes and area boundaries
    3. Produce sounds triggered by game events -- in a manner that does not cause the
       game to pause.
    4. Communicate with the player using text.
    5. Include a state machine written in assembly language.
    6. Respond to user input (from buttons).

## Game Description
The main idea of this game is to avoid all the moving bars, and touch the top
of the playing field to win. A player has three lives. Every time the blue
square touches a bar, the player looses a life. If the player touches tree
times the moving bars, the player lost. However, if the square touches the top
of the playing field, the player wins. You will know when you win or loose
beceause a new screen wil show up.

## Instructions
To compile the code and start playing, open the shell by M-x shell and type
make load.
```bash
make load
```

The board has four buttons. These buttons are used to move the little square.

## Libraries
Several libraries are provided.
They can be installed by the default production of Makefile in the
repostiory's root directory, or by a "$make install" in each of their subdirs.

    1. TimerLib: Provides code to configure Timer A to generate watchdog timer
       interrupts at 250 Hz
    2. p2SwLib: Provides an interrupt-driven driver for the four switches on the LCD
       board and a demo program illustrating its intended functionality.
    3. lcdLib: Provides low-level lcd control primitives, defines several fonts, and
       a simple demo program that uses them.
    4. shapeLib: Provides an translatable model for shapes that can be translated and
       rendered as layers.
    5. circleLib: Provides a circle model as a vector of demi-chord lengths,
       pre-computed circles as layers with a variety of radii, and a demonstration
       program that renders a circle.
