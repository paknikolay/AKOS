# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /home/nikolai/Загрузки/clion-2017.1/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/nikolai/Загрузки/clion-2017.1/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/nikolai/Life(P)"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/nikolai/Life(P)/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/L.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/L.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/L.dir/flags.make

CMakeFiles/L.dir/live.c.o: CMakeFiles/L.dir/flags.make
CMakeFiles/L.dir/live.c.o: ../live.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/nikolai/Life(P)/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/L.dir/live.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/L.dir/live.c.o   -c "/home/nikolai/Life(P)/live.c"

CMakeFiles/L.dir/live.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/L.dir/live.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/nikolai/Life(P)/live.c" > CMakeFiles/L.dir/live.c.i

CMakeFiles/L.dir/live.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/L.dir/live.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/nikolai/Life(P)/live.c" -o CMakeFiles/L.dir/live.c.s

CMakeFiles/L.dir/live.c.o.requires:

.PHONY : CMakeFiles/L.dir/live.c.o.requires

CMakeFiles/L.dir/live.c.o.provides: CMakeFiles/L.dir/live.c.o.requires
	$(MAKE) -f CMakeFiles/L.dir/build.make CMakeFiles/L.dir/live.c.o.provides.build
.PHONY : CMakeFiles/L.dir/live.c.o.provides

CMakeFiles/L.dir/live.c.o.provides.build: CMakeFiles/L.dir/live.c.o


# Object files for target L
L_OBJECTS = \
"CMakeFiles/L.dir/live.c.o"

# External object files for target L
L_EXTERNAL_OBJECTS =

L : CMakeFiles/L.dir/live.c.o
L : CMakeFiles/L.dir/build.make
L : CMakeFiles/L.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/nikolai/Life(P)/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable L"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/L.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/L.dir/build: L

.PHONY : CMakeFiles/L.dir/build

CMakeFiles/L.dir/requires: CMakeFiles/L.dir/live.c.o.requires

.PHONY : CMakeFiles/L.dir/requires

CMakeFiles/L.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/L.dir/cmake_clean.cmake
.PHONY : CMakeFiles/L.dir/clean

CMakeFiles/L.dir/depend:
	cd "/home/nikolai/Life(P)/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/nikolai/Life(P)" "/home/nikolai/Life(P)" "/home/nikolai/Life(P)/cmake-build-debug" "/home/nikolai/Life(P)/cmake-build-debug" "/home/nikolai/Life(P)/cmake-build-debug/CMakeFiles/L.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/L.dir/depend

