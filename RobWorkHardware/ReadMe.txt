Many of the packages within RobWorkHardware only compiles on specific platforms. When in need of
specific device then enable it in the config.cmake.template file.


examples:

Compilation, Win32 MinGW example
- Copy 'config.cmake.template' to 'config.cmake' and make your configuration 
  if default is not good enough 
- You should make sure to create the build tree from within a subfolder
-- create "release" and go to "release"
-- execute 'cmake -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release ../..'
-- execute cmakesetup to configure the CACHED variables if needed 
-- execute make all

Compilation, Linux GCC example
- Copy 'config.cmake.template' to 'config.cmake' and make your configuration  
  if default is not good enough 
- You should make sure to create the build tree from within a subfolder
-- create "release" and go to "release"
-- execute 'cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Release ../..'
-- execute ccmake to configure the CACHED variables if needed 
-- execute make all
