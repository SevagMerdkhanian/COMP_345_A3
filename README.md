# COMP345_Project - Tower Defense Game
Armen Jabamikos - 40273685

Gevorg Markarov - 40245363

Shahe Bannis - 40286754

Sevag Merdkhanian - 40247912

Hrag Bankian - 40245363

## Install Raylib on Visual Studio

1. Download Raylib from https://github.com/raysan5/raylib/releases/tag/5.0
   - For windows64 use raylib-5.0_win64_msvc16.zip
2. Extract the downloaded file.
3. Open the project on visual studio.
4. Right click on the project name in the solution explorer and click on properties.
5. Check if the following steps are done:
   - Go to C/C++ -> General -> Additional Include Directories and add the path to the include folder in the extracted folder.
   - Go to Linker -> General -> Additional Library Directories and add the path to the lib folder in the extracted folder.
   - Go to Linker -> Input -> Additional Dependencies insert "raylib.lib;winmm.lib".

## Documentation
- You can access it in the Documented_COMP_345_Project/html/index.html
