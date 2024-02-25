# RP6502 Conway's Game of Life using the RP6502 CC65 VSCode

This is an example project using the Picocomputer 6502 cc65 VSCode scaffolding for John Horton Conway's Game of Life.

Credit for the original source code to rumbledethumps: https://github.com/rumbledethumps/life

Rumbledethumps' original code was for the llvm-mos-sdk compiler; here it has been ported 
to the cc65 compiler.

Be patient: A life generation takes about 100-seconds (@ 8-Mhz-6502) for this cc65 code.
Screen size is 320 x 180 x 1bpp. Read code-comments for more information. 
This code is an excellent basis for studying code optimization and computational efficiency.

### References:
* [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
* [Glider in Game of Life](https://en.wikipedia.org/wiki/Glider_(Conway's_Game_of_Life))


### Linux Tools Install:
 * [VSCode](https://code.visualstudio.com/). This has its own installer.
 * A source install of [CC65](https://cc65.github.io/getting-started.html).
 * The following tools installed from your package managers:
    * `sudo apt-get install cmake python3 pip git build-essential`
    * `pip install pyserial`

### Windows Tools Install:
 * [VSCode](https://code.visualstudio.com/). This has its own installer.
 * A source install of [CC65](https://cc65.github.io/getting-started.html).
   Do not skip the step about adding bin to your path.
 * Install python by typing `python3` which will launch the Microsoft Store
   where you start the install. If python runs, this has already been done,
   exit python with Ctrl-Z plus Return.
 * Install the python serial library with `pip install pyserial`.
 * `winget install -e --id Kitware.CMake`.
 * `winget install -e --id GnuWin32.Make`.
    Add "C:\Program Files (x86)\GnuWin32\bin" to your path.
 * `winget install -e --id Git.Git`.

### Getting Started:
Clone the repository and build the project with VSCode.

```
$ git clone [path_to_github]
$ cd [to_where_it_cloned]
$ code .
```

Install the extensions and choose the default or obvious choice if VSCode
prompts you.

You can build with F7. Running a program is done with "Run Build Task..."
CTRL-SHIFT-B. If the default communications device doesn't work, edit ".rp6502"
in the project root folder. This file will be created the first time you
"Run Build Task..." and will be ignored by git.

Edit CMakeLists.txt to add new source and asset files. It's
pretty normal C/ASM development from here on.
