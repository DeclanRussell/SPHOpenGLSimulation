*******************
* sdflib binaries *
*******************
 
This library is distributed under the CGPL - see licence.txt or licence.rtf
 
Headers are located in the "include" directory
The compiled libraries are in the "lib" directorys
 
All libraries are compiled in 32 bits,
sdf-lite-vc90-mt-s.lib is compiled with a static run-time library under vc9 (Visual Studio 2008)
sdf-lite-vc90-mt-sgd.lib is compiled with a static run-time library under vc9 (Visual Studio 2008) in debug configuration
sdf-lite-vc90-mt.lib is compiled with a dynamic (DLL) run-time library under vc9 (Visual Studio 2008)
sdf-lite-vc90-mt-gd.lib is compiled with a dynamic (DLL) run-time library under vc9 (Visual Studio 2008) in debug configuration
 
*********************
* Using the library *
*********************s
Using Linux and Qt:
-Copy the right lib files from the lib folder (libsdf-lite-linuxgcc-mt-debug.a and libsdf-lite-linuxgcc-mt-release.a) and put them in a folder "lib" in your project directory.
-Copy the header file signed_distance_field_from_mesh.hpp in the include directory and put it in your include directory.
-For copyright reasons, you also need to copy the licence files (licence.txt and licence.rtf)
-In your project settings, add the following lines
 
LIBS += -L ./lib/ -lsdf-lite-linuxgcc-mt-release
HEADERS += include/signed_distance_field_from_mesh.hpp 