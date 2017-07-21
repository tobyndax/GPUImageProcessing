# GPUImageProcessing
#GPU accelerated Image Processing Library

The project is licensed under the MIT license.


##Third Party Software
This software uses LodePNG for PNG loading
Copyright (c) 2005-2016 Lode Vandevenne

Many thanks for an excellent piece of software

SDL2, this is used for viewing results.

#Build instructions
##Mac OS X
Install SDL2 via for instance brew, (brew install sdl2)

Run cmake in the src directory on CMakeLists.txt or cmake -D CMAKE_BUILD_TYPE=Release CMakeLists.txt for a release build.

##Windows Visual Studio 2017
Right click the GPUImageProcessing folder, select open folder (VS).
The project should configure itself, select the startup app and build.

SDL dll's are included in the repo for Windows.
