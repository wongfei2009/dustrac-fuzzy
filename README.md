## Dust Racing 2D

<table>
  <tr>
    <td>
      <img src="https://cloud.githubusercontent.com/assets/2587783/7400283/69f106fa-eec4-11e4-9014-76c086f54d89.jpg" width="100%"></img>
    </td>
    <td>
      <img src="https://cloud.githubusercontent.com/assets/2587783/7400318/956180d0-eec4-11e4-9d3a-96e83d4b0e52.jpg" width="100%"></img>
    </td>
  </tr>
</table>

<a href="https://www.youtube.com/watch?feature=player_embedded&v=_0f7rpGYE5g">Video of the game</a>

Dust Racing (Dustrac) is a tile-based, cross-platform 2D racing game written
in Qt (C++) and OpenGL.

Dust Racing comes with a Qt-based level editor for easy level creation.
A separate engine, MiniCore, is used for physics modeling.

## Features

* 1-2 human players againts 11 challenging computer players
* 3 difficulty settings: Easy, Medium, Senna
* Split-screen two player game (vertical or horizontal)
* Game modes: Race, Time Trial, Duel
* 2D graphics with some 3D objects
* Smooth game play and physics
* Multiple race tracks
* Finishing in TOP-6 will unlock the next race track
* Star ratings based on the best positions on each race track
* Easy to create new race tracks with the level editor
* Engine and collision sounds
* Pit stops
* Runs windowed or fullscreen
* Portable source code using CMake as the build system
* Will be forever completely free

## License

Dust Racing source code is licensed under GNU GPLv3. 
See COPYING for the complete license text.

Dust Racing includes the source code for the GLEW library.
See src/game/MiniCore/Graphics/glew/glew.h for the license text.

Dust Racing includes the source code for the GLM library.
See src/game/MiniCore/Graphics/glm/ogl-math/glm/glm.hpp for the license text.

All image files, except where otherwise noted, are licensed under
CC BY-SA 3.0: http://creativecommons.org/licenses/by-sa/3.0/

## Playing

### Controls

The default controls for player one (the pink race car):

Accelerate | Brake | Turn left | Turn right
---------- | ----- | --------- | ----------
Up         | Down  | Left      | Right

The default controls for player two (the grey race car):

Accelerate | Brake | Turn left | Turn right
---------- | ----- | --------- | ----------
W          | S     | A         | D

The key configuration and game mode can be changed in the settings menu.

**ESC** or **Q** exits the race and also the current menu.

**P** pauses the game.

### Races

In the race modes there are always 12 cars. By finishing in TOP-6 a new track
will be unlocked.

The record times and best positions are stored separately for each lap count.

### Pit stops

Your tires will wear out as the race progresses. This causes more and more sliding.

Fortunately there's a pit (the yellow rectangle). 
By stopping on the pit your tires will be repaired.

### Custom track files

Dust Racing searches for race tracks also in ~/DustRacingTracks/
where you can place your own race tracks.

## Command line options

--lang [lang] forces the language. Currently available: fi, cs, de, it, fr.

E.g. ./dustrac-game --lang it

## Building the project

Please refer to the INSTALL document for build/install instructions if you're
going to build Dust Racing 2D from sources.

-- Jussi Lind <jussi.lind@iki.fi>

