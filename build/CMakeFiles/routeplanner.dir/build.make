# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/isma/git_repos/DA2425-routeplanner

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/isma/git_repos/DA2425-routeplanner/build

# Include any dependencies generated for this target.
include CMakeFiles/routeplanner.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/routeplanner.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/routeplanner.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/routeplanner.dir/flags.make

CMakeFiles/routeplanner.dir/codegen:
.PHONY : CMakeFiles/routeplanner.dir/codegen

CMakeFiles/routeplanner.dir/src/main.cpp.o: CMakeFiles/routeplanner.dir/flags.make
CMakeFiles/routeplanner.dir/src/main.cpp.o: /home/isma/git_repos/DA2425-routeplanner/src/main.cpp
CMakeFiles/routeplanner.dir/src/main.cpp.o: CMakeFiles/routeplanner.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/isma/git_repos/DA2425-routeplanner/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/routeplanner.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/routeplanner.dir/src/main.cpp.o -MF CMakeFiles/routeplanner.dir/src/main.cpp.o.d -o CMakeFiles/routeplanner.dir/src/main.cpp.o -c /home/isma/git_repos/DA2425-routeplanner/src/main.cpp

CMakeFiles/routeplanner.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/routeplanner.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/isma/git_repos/DA2425-routeplanner/src/main.cpp > CMakeFiles/routeplanner.dir/src/main.cpp.i

CMakeFiles/routeplanner.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/routeplanner.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/isma/git_repos/DA2425-routeplanner/src/main.cpp -o CMakeFiles/routeplanner.dir/src/main.cpp.s

# Object files for target routeplanner
routeplanner_OBJECTS = \
"CMakeFiles/routeplanner.dir/src/main.cpp.o"

# External object files for target routeplanner
routeplanner_EXTERNAL_OBJECTS =

routeplanner: CMakeFiles/routeplanner.dir/src/main.cpp.o
routeplanner: CMakeFiles/routeplanner.dir/build.make
routeplanner: CMakeFiles/routeplanner.dir/compiler_depend.ts
routeplanner: CMakeFiles/routeplanner.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/isma/git_repos/DA2425-routeplanner/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable routeplanner"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/routeplanner.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/routeplanner.dir/build: routeplanner
.PHONY : CMakeFiles/routeplanner.dir/build

CMakeFiles/routeplanner.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/routeplanner.dir/cmake_clean.cmake
.PHONY : CMakeFiles/routeplanner.dir/clean

CMakeFiles/routeplanner.dir/depend:
	cd /home/isma/git_repos/DA2425-routeplanner/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/isma/git_repos/DA2425-routeplanner /home/isma/git_repos/DA2425-routeplanner /home/isma/git_repos/DA2425-routeplanner/build /home/isma/git_repos/DA2425-routeplanner/build /home/isma/git_repos/DA2425-routeplanner/build/CMakeFiles/routeplanner.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/routeplanner.dir/depend

