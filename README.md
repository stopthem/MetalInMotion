# MetalInMotion

Developed with Unreal Engine 5.1.

A rolling ball demo from https://www.udemy.com/course/pro-unreal-engine-game-coding/.

This is basically my first Unreal project.\
I mostly tried to learn the engine and c++ without much polishing on the game.

Things i changed or added:
- Movement was not normalized
- Project was not using enhanced input so i implemented it.
- Dash was all over the place, it was not consistent, wrong direction and had no polishing whatsoever. I added vfx, angular velocity and made it consistent.
- Grounded was handled in ball bearing base class with sphagetti code and i moved it to playerballbearing and cleaned it. I changed grounded functionality(sphere trace) and made it consistent.
- Player ball has a desired angular velocity in air based on input. Basically ball turns towards input direction on air.
- Added bhop.
- Level end check was handled in every goal, i moved it to game mode and cleaned it.
- Ball bearing goals has no ball bearing vfx and has ball bearing vfx.

There is flaws of course, like scripts are not correctly foldered, not using TObjPtr<>, sounds can be better, scripts can be more readable, can use much much more polishing.


https://user-images.githubusercontent.com/68704994/222407297-e4e5191d-c6e2-47e7-9c4d-f6cc9bbcd560.mp4


