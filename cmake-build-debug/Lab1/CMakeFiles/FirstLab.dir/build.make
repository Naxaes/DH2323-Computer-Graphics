# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug

# Include any dependencies generated for this target.
include Lab1/CMakeFiles/FirstLab.dir/depend.make

# Include the progress variables for this target.
include Lab1/CMakeFiles/FirstLab.dir/progress.make

# Include the compile flags for this target's objects.
include Lab1/CMakeFiles/FirstLab.dir/flags.make

Lab1/CMakeFiles/FirstLab.dir/skeleton.cpp.o: Lab1/CMakeFiles/FirstLab.dir/flags.make
Lab1/CMakeFiles/FirstLab.dir/skeleton.cpp.o: ../Lab1/skeleton.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Lab1/CMakeFiles/FirstLab.dir/skeleton.cpp.o"
	cd /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/Lab1 && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/FirstLab.dir/skeleton.cpp.o -c /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/Lab1/skeleton.cpp

Lab1/CMakeFiles/FirstLab.dir/skeleton.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FirstLab.dir/skeleton.cpp.i"
	cd /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/Lab1 && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/Lab1/skeleton.cpp > CMakeFiles/FirstLab.dir/skeleton.cpp.i

Lab1/CMakeFiles/FirstLab.dir/skeleton.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FirstLab.dir/skeleton.cpp.s"
	cd /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/Lab1 && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/Lab1/skeleton.cpp -o CMakeFiles/FirstLab.dir/skeleton.cpp.s

# Object files for target FirstLab
FirstLab_OBJECTS = \
"CMakeFiles/FirstLab.dir/skeleton.cpp.o"

# External object files for target FirstLab
FirstLab_EXTERNAL_OBJECTS =

Lab1/FirstLab: Lab1/CMakeFiles/FirstLab.dir/skeleton.cpp.o
Lab1/FirstLab: Lab1/CMakeFiles/FirstLab.dir/build.make
Lab1/FirstLab: /usr/local/lib/libSDLmain.a
Lab1/FirstLab: /usr/local/lib/libSDL.dylib
Lab1/FirstLab: Lab1/CMakeFiles/FirstLab.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable FirstLab"
	cd /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/Lab1 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FirstLab.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Lab1/CMakeFiles/FirstLab.dir/build: Lab1/FirstLab

.PHONY : Lab1/CMakeFiles/FirstLab.dir/build

Lab1/CMakeFiles/FirstLab.dir/clean:
	cd /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/Lab1 && $(CMAKE_COMMAND) -P CMakeFiles/FirstLab.dir/cmake_clean.cmake
.PHONY : Lab1/CMakeFiles/FirstLab.dir/clean

Lab1/CMakeFiles/FirstLab.dir/depend:
	cd /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/Lab1 /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/Lab1 /Users/tedkleinbergman/Programming/DH2323-Computer-Graphics/cmake-build-debug/Lab1/CMakeFiles/FirstLab.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Lab1/CMakeFiles/FirstLab.dir/depend

