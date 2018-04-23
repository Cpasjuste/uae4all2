Uae4all2
A fast and optimized Amiga Emulator

Features: AGA/OCS/ECS, 68020 and 68000 emulation, harddisk-support, WHDLoad-support, Chip/Slow/Fast-mem settings, savestates, vsync, most games run fullspeed

Authors:
Chui, john4p, TomB, notaz, Bernd Schneider, Toni Wilen, Pickle, smoku, AnotherGuest, Anonymous engineer, finkel, Lubomyr, pelya

Android port by Lubomyr, android libSDL by pelya

Vita port by Cpasjuste, additional features by rsn8887 with help from ScHlAuChi, VitaSDL by Cpasjuste based on Vita2D by Xerpi and frangarCJ and others, shader support by Cpasjuste and frangarcj, VitaSDK by The VITASDK Team

Android-port info
-----------------
On-screen scheme

4 3 ( YX)
2 1 ( BA)

'1'- <Button A> autofire [HOME]
'2'- <Button B> Right mouse click [END]
'3'- <Button X> joystick fire/left click [PAGEDOWN]
'4'- <Button Y> [PAGEUP]
'5' <Button L> [SDLK_F13]
'6' <Button R> [SDLK_RCTRL]
old textUI vkeybd - '2'+'4'

SDL keycode action:
F15 textUI vkeybd
F12 - emulator menu toggle
F11 - change input method

Please put kickstarts files in android/data/pandora.uae4all.sdl/kickstarts directory.
Files must be named as kick13.rom kick20.rom kick31.rom

Vita-port info
--------------
VITA-EXCLUSIVE FEATURES:

- Full speed emulation of most Amiga games with zero frameskip
- Large, complete virtual keyboard with adjustable position and adjustable transparency
- Three fully mappable custom control sets: map Vita buttons to any Amiga joystick or key input, three custom control sets supported
- Multiple hard-file support: up to 4 hdf files (size <2GB each) can be mounted simultaneously
- Multiple controller support on Vita TV: up to four paired controllers can be fully mapped
- Analog mouse control: control the mouse using the analog stick with adjustable sensitivity
- Shader support: default "sharp_bilinear_simple" uses integer pre-scale for sharp, undistorted pixels with minimum blur, even if game is zoomed to fill the full Vita screen
- Custom autofire: any mapped Vita button can have autofire with adjustable rate
- Savestate with hard-file support: state can be saved even when hard-files are used
- Additional emulator settings: sprite-sprite collisions can be enabled, blitter settings can be changed
- Adjustable stereo separation
- Bluetooth keyboard and mouse support
- Sticky virtual keyboard modifiers: allows keyboard combos like CTRL-C to be entered easily
- Touchpad style pointer controls using the front touch panel and, optional, the rear touch panel. Touch pointer speed can be adjusted with the Mouse Speed option.

NOTES:

- Bios: Please put kickstart files in "ux0:/data/uae4all/kickstarts" directory. Files must be named as kick13.rom, kick20.rom, kick31.rom

- Speed: Most games run full speed. Some slow Amiga 1200 and WHDLoad games actually run faster if the CPU speed is reduced from 14 MHz to 7 MHz, because they rely more on custom chips than CPU. Some graphics glitches in Lionheart WHDLoad inside the caves are fixed by this.

- Config File: If you have problems with settings not saving etc, please delete or rename your config file in the folder "ux0:/data/uae4all" and restart the emu. I heard this can fix many problems when you are replacing an older version of this emulator with a newer version. Lots of things are changing in the config file between releases.

- Display: The sharp_bilinear_simple shader, on by default, creates a sharp pixel image with minimal blurring. It applies a maximum integer pre-scale first. Then it scales only the small remainder using the builtin bilinear filter, keeping the aspect ratio intact.
Many Amiga games fit the Vita screen very nicely. I urge everyone to try Chaos Engine with a screen setting of 320*200 and the sharp_bilinear_simple shader. The picture almost fills the whole screen and is very pretty with no artifacts or blurriness. The picture can be moved using R+Start+Dpad to center it on the Vita screen. It runs smooth with zero frameskip.
Selecting a 'Preset Width' of 320, 352 or 384 (max overscan) results in lo-res screen emulation, good for most games, while 640, 704, or 768 results in hi-res screen emulation, good for use with Workbench or some text-based games.

- Blitter: The Blitter settings can help with graphics glitches in some games. For example, Rick Dangerous graphics are fixed by setting Blitter to "immediate" instead of "normal," and rebooting the game. Since it works much better, "immediate" is now the default.

- Sprite Collisions: Some games require sprite-to-sprite collisions. They can be turned on in the "more options" menu. The only game I know of that requires this is SpaceTaxi 3.

- Multiplayer: Up to four controllers on Vita TV are supported and can be fully mapped. The mouse can be switched off because it interferes with the other joystick that controls player 2 in most games. If you pair new controllers and they do not seem to work, scan for them in the "custom controls" menu by pressing L. At the bottom it will say how many controllers have been found.

- Sound: For use with headphones, a stereo separation setting of 50 sounds better than the default setting of 100. The original Amiga had completely disjoint left/right channels. This can sound quite jarring on headphones. Concerning sound frequency, on the Vita, the default of 44.1 kHz gives by far the best results.

- Building from Source: The dependencies are
VitaSDK: http://vitasdk.org (toolchain and Vita system headers etc.)
SDL_ttf: https://github.com/Cpasjuste/SDL_ttf
SDL-Vita (v1.2 with 4 controller support and with Bluetooth support, from December 24, 2017 or later): https://github.com/rsn8887/SDL-Vita/tree/SDL12
SDL-Image: https://github.com/Cpasjuste/SDL_image-1.2.12
Install VitaSDK. Clone all the above repositories and install their libraries using "make -f Makefile.psp2 install" for each one. Cd into a separate build directory outside uae4all2 tree. Type "ccmake <uae4all2 dir>". Then press c (configure). In the release type form field, type "Release" and enter. Then press c then g. Then type "make uae4all2.vpk".

Note:
The source depends on FrangarCJ's Vita2d fbo branch with shader support: https://github.com/Cpasjuste/vita2dlib/tree/fbo
and FrangarCJ's vita-shader-collection gtu branch: https://github.com/frangarcj/vita-shader-collection/tree/gtu
Those are both included as binaries and linked automatically.

Select = Toggle menu
Start = Toggle virtual keyboard
R+Start+Dpad up/down = Move screen up/down
R+Start+Dpad left/right = Change screenmode (to zoom in on games with borders)
Right analog stick = Analog mouse (can switch this to left stick in menu)
Left analog stick = Amiga joystick directions
Dpad = Amiga joystick directions

When custom controls are off:
L/R shoulder buttons = Mousebuttons
(Shown below are control presets 1 (default) / 2 / 3 / 4)
Square = Autofire (default) / Fire / Autofire / Fire
Cross = Fire (default) / Autofire / Up (Jump) / Up (Jump) 
Triangle = Space (default) / Space / Fire / Autofire
Circle = Secondary Fire (used only in a few games)
R+Square = Ctrl
R+Circle = Left Alt
R+Cross = Help
L+Square = Left mouse
L+Circle = Right mouse
R+Dpad = Arrow keys

When custom controls are on:
L+Start = Toggle between custom control set number 1, 2, and 3

Virtual keyboard controls:
Start = Toggle virtual keyboard
Right analog stick up/down = Move virtual keyboard up and down
Right analog stick left/right = Change virtual keyboard transparency
Cross = Press selected key
Square = Backspace
Triangle = Toggle shift
Circle = Turn off all sticky keys (ctrl, alt, amiga, and shift)

Touch controls:
single short tap = left mouse click
single short tap while holding a second finger down = right mouse click
single finger drag = move the mouse pointer
dual finger drag = drag'n'drop (left mouse button is held down)

CHANGELOG:
1.66

- Prevent accidental clicks when moving pointer using touch

1.65

- eliminate transparent keyboard slowdown
- fix joystick port setting affecting custom controls unless set to 'port1'

1.64

- improved 'normal' blitter mode (fixes flicker in PP Hammer and Spindizzy Worlds)

1.63

- implemented three-finger drag as right-button drag (useful to hold-down the right mouse button). Fingers have to be somewhat apart from each other, otherwise they are recognized as single or dual touch by the Vita.

1.62

- re-compiled in safe-mode
- fix empty directory listing of uma0:

1.61

- re-compiled in unsafe-mode

1.60

- support for uma0: added
- prevent suspend mode because it can corrupt hdf files

1.59

- implemented touchpad-style pointer controls. Touch controls are configured under 'More Options/Touch.' Choices are 'Touch Off', 'Front only', or 'Front and Back.' The default is to use only the front panel. Both panels work the same, using relative touch mode. How far a certain finger motion moves the pointer depends on the Mouse Speed setting, which also affects how fast the analog stick moves the mouse.

1.58

- fix 'movec pcr,d0' not throwing exception on 68020 (fixes WHDload 18.3)
- recompiled with latest SDL 1.2 library supporting full analog joystick range

1.57

- Sticky virtual keyboard keys implemented. The alt, ctrl, amiga, and shift virtual keyboard keys are now sticky. Press them once to enable and another time to let go of the key. Key combos like "Amiga-Q" can now be entered using the virtual keyboard. The circle button quickly un-sticks all keys.

1.56

- fix keys being pressed when pressing Vita buttons (introduced in 1.55)

1.55

- support for Bluetooth keyboard and mouse added. The Vita doesn't pair with all Bluetooth keyboard/mouse combos. As a rule, if the keyboard works to highlight bubbles with cursor keys, it should also work in UAE4All. Mouse + keyboard tested working with the "Jelly Comb Mini Bluetooth Keyboard With Mouse Touchpad, ASIN:B06Y56BBYP."

1.54

- more choices for setting mouse speeds in steps of 0.25 now
- a button can be mapped to "slow mouse" in custom controls. When that button is held down, the mouse pointer speed is greatly reduced. This is useful for precisely positioning the cursor.

1.53

- fix analog stick angular zone sizes (22.5 degrees now)

1.52

- improved joystick diagonals and deadzone

1.51

-fix autofire for ply #2 w/o custom controls

1.50

- fix analog deadzone (broken in last release)

1.49

-improved analog mouse deadzone (radial and scaled)

1.48

- cleaned up display menu
- added option to choose menu background

1.47

- new menu background
- fix swapped "Quote"<->"Backquote" overlay keyboard keys

1.46

- never save autofire button to reduce confusion
- fixed menu navigation bug in HDF selection

1.45

- added option to use autofire with custom configs, can choose one Vita button that will have autofire, or set it to 'none' (default) to turn it off

1.44

- improved contrast of virtual keyboard overlays (thanks to ScHlAuChi for new images)
- added option to choose between US, UK and German virtual keyboard overlays (thanks to ScHlAuChi for updated images)

1.43

- full support for up to 4 controllers on Vita TV that can be fully mapped individually
- filelists (adf, hdf, etc) are now sorted alphabetically with directories at the top
- added option to turn off mouse emulation because it often interferes with second player (port 0 joystick) when playing multiplayer on the Vita TV
- added option to turn on sprite-sprite collisions, useful for Spacetaxi 3
- changed blitter default to "immediate" it seems to be more compatible

1.42

- added two player support on Vita TV, second paired Dualshock controller controls the 'other' joystick, only works if port is not set to 'both'
- fixed a crash bug when changing audio parameters while sound was playing

1.41

- assembler optimizations enabled by Cpasjuste, emulator is faster now!

1.40

 - disabled the menu entries 'screenwidth' and 'stylus offset' that did nothing on the Vita. To show the borders in overscan games like Settlers, simply use the menu option 'Preset Width' and set it to >320 for lores, (or >640 for hires). There are choices 352, 384, 704 and 768 that cover the full overscan range.

1.39

- fix sound setting changes not being applied until restart
- fix sound distortion for stereo separation settings 0 (mono) and 25%

1.38

- added stereo separation setting to select between 0 (mono) 25%, 50%, 75% and 100% (stereo, default, original Amiga sound)
- added the blitter options 'normal, immediate, and improved' default is 'normal.' Might help with some games

1.37

- some fixes to savestates. Now savestates work even with no disk inserted in df0. Savestate file name is now the first available name among df0, boot-hd, hdf2, hdf3, hdf4, df1, df2, df3.

1.36

- added measures to prevent file corruption: Disabled PS button because it caused corruption of hdf files; display a warning about file corruption if the Amiga is writing to hdf while the user tries to exit the program
- fix hdf size checking
- fixed up some menu text
- adjusted quickswitch screenmodes by two pixels

1.35

- now upto four hdf files can be mounted. From my own testing, it looks like 2 Gb is the hdf size limit. Geometry for each file can be changed by manually editing the config file, default is sectors:surfaces:reserved:blocksize=32:1:2:512 for files <1 GB, 32:2:2:512 for files >1Gb and 32:4:2:512 for files >2Gb. Only files <=2 Gb seem to work.

1.34

- hires support (e.g. 640*256), switchable in display menu. (Select a screen mode with "640" as width in the first option of the display menu, and the Workbench will be rendered with every pixel. A width of "320" reverts back to old behavior.)

1.33

- implemented three fully mappable custom configs, configurable in controls menu. Quick switch between them using "L+Start"
- allow the "other joystick" to be mapped in custom controls to the Dpad or any buttons. This only works if ports is not set to "both". Works great for Llamatron in dual joystick mode!

1.32

- virtual keyboard transparency adjust now toggles between four values, two of which cause zero slow-down when keyboard is displayed.
- resolution/zoom quick switch with R+Start+Dpad left/right is now more responsive

1.31
- Allow menu quit from within sub-menus using 'select'

1.30
- updated autocentering of analog sticks so that both sticks are always auto-centered, not just the one that controls the mouse

1.29

- Changed controls. The virtual keyboard is too useful to put it on an obscure combo. Start Button now toggles the virtual keyboard. When keyboard is active, right analog stick then controls the keyboard's vertical position and transparency. When keyboard is off, use R+Start+Dpad to move screen up and down and to quick switch between common resolutions. This used to be just Start+Dpad. Now it is R+Start+Dpad.

1.28

- bugfix: the virtual keyboard sometimes sent joystick button presses to the emulator when selecting keys. This has been fixed.

1.27

- virtual keyboard hotkeys added, square = backspace, triangle = toggle shift
- virtual keyboard controls are now more responsive

1.26

- virtual keyboard transparency is now adjustable (START+dpad left/right)
- fixed bug in virtual keyboard initialization where the keyboard send an endless string of ' symbols on boot up before it was first opened

1.25

- vastly improved virtual keyboard: The image is now bigger, supports more keys including the shift modifier, and is transparent (thanks to ScHlAuChi for the new keyboard image and many ideas)
- Fixed virtual keyboard not reacting when custom dpad mode was active
- The virtual keyboard can now be moved up and down using start+dpad up and start + dpad down.

1.24

- virtual keyboard (mapped to "hold start then press select")
- labels in custom control menu now reflect Vita controller
- default analog mouse control deadzone increased from 100 to 1000 to fix mouse pointer drift (can be changed in menu)
- default joystick port set to 1, since the mouse is always on (can be changed in menu)

1.23
- added analog joystick deadzone setting
- implemented autocentering to fix mouse pointer drift on some Vitas. Centering happens on program start and whenever the menu is opened
- START+DPAD left/right for quick switching between different zoomed screenmodes, useful for games like Chaos Engine. Use START+DPAD up/down to center screen after quick switching.

1.22
- fixed broken analog mouse control dead zone and improved mouse analog sensitivity

1.21
- moved shader config to display menu

1.20:
- added analog mouse support with menu option to choose left or right stick for mouse control
- new menu items in misc menu:
shader choice (default: sharp_bilinear_simple)
choice which analog stick to use for mouse
- disabled broken gp2x stylus mode and mouse emu mode
- L/R are mouse buttons now
- screen movement switched from L+R+DPAD to START+DPAD
- START does not switch input modes anymore (mouse is always enabled)
- updated Readme.txt with new Vita control scheme
- fixed misc menu entries for shader and analog mouse
- custom controls menu is now more snappy

<1.20:
- perfect 2x scaling support to reduce pixel wobble
- HD Dir fix (supports one HD Dir & one HDF simultaneously, but max hdf size is still 1 Gb)
- Shader support implemented by Cpasjuste
