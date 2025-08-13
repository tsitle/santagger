# Building Sandy Tagger 

## Prerequisites

### Rocky Linux / Fedora

```
$ sudo dnf install cmake
```

#### For building a statically linked executable

**Note:** By default, the external libraries required for decoding audio are not available
as archive libraries (which are required for statically linking them) under Rocky Linux / Fedora.

#### For building a dynamically linked executable

Optional external libraries:

```
$ sudo dnf install mpg123-devel
    (or 'libmad-devel')
$ sudo dnf install libvorbis-devel libogg-devel
$ sudo dnf install zlib-devel
```

### Debian / Ubuntu

```
$ sudo apt install cmake
```

#### For building a statically linked executable

**Note:** When building a statically linked executable, you need to use 'libmad0-dev' instead
of 'libmpg123-dev', because 'libmpg123-dev' doesn't come with the required archive library ('libmpg123.a').

Optional external libraries:

```
$ sudo apt install libmad0-dev
$ sudo apt install libvorbis-dev libogg-dev
$ sudo apt install zlib1g-dev
```

#### For building a dynamically linked executable

Optional external libraries:

```
$ sudo apt install libmpg123-dev
              (or 'libmad0-dev')
$ sudo apt install libvorbis-dev libogg-dev
$ sudo apt install zlib1g-dev
```

### macOS

```
$ brew install cmake
```

Optional external libraries:

```
$ brew install mpg123
    (or 'mad')
$ brew install libvorbis libogg
$ brew install zlib
```

### MS Windows

MinGW with GCC and CMake:

- install MinGW-w64 from [MSYS2](https://www.msys2.org/)
- launch the UCRT64 environment
- install the MinGW-w64 toolchain:  
  `$ pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain`
- add the path of your MinGW-w64 bin folder to the Windows PATH environment variable by using the following steps:
    - in the Windows search bar, type "Settings" to open your Windows Settings
    - search for "Edit environment variables" for your account
    - In your User variables, select the "Path" variable and then select "Edit"
    - select "New" and add the MinGW-w64 destination folder you recorded during the installation process to the list.  
      If you used the default settings above, then this will be the path: "C:\msys64\ucrt64\bin"
    - select "OK", and then select "OK" again in the Environment Variables window to update the PATH environment variable
    - you have to reopen any console windows for the updated PATH environment variable to be available
- check that your MinGW-w64 tools are correctly installed and available, open a new Command Prompt and type:
    ```
    gcc --version
    g++ --version
    gdb --version
    ```
- install CMake:  
  `$ pacman -S mingw-w64-ucrt-x86_64-cmake`

Optional external libraries:

```
$ pacman -S mingw-w64-ucrt-x86_64-mpg123
    (or 'mingw-w64-ucrt-x86_64-libmad')
$ pacman -S mingw-w64-ucrt-x86_64-libvorbis mingw-w64-ucrt-x86_64-libogg
$ pacman -S mingw-w64-ucrt-x86_64-zlib
```

## Building

### CMake Options

- `BUILD_FOR_VALGRIND`: build with compiler flags for Valgrind (default: `OFF`)
- `STRIP_AFTER_BUILD`: strip debug symbols etc. from executable/library after build (default: `OFF`)
- `BUILD_SHARED_LIBS`: build dynamically linked executable/library (default: `ON`)

### Bash Scripts for macOS / Linux

For convenience, there are some Bash scripts for building the project on macOS / Linux.

Each script can print out a list of available options by using the `--help` option.

For example, building the project can be done like this:

```
Build type = release:
$ ./za-cmake.sh && ./zb-build.sh
```

Building with Valgrind compiler flags can be done like this:

```
Build type = release:
$ ./za-cmake.sh release vg static && ./zb-build.sh vg_release_stat

Build type = debug:
$ ./za-cmake.sh debug vg static && ./zb-build.sh vg_debug_stat
```

Building all executables (the application plus all Unit Tests) with the Debug build type can be done like this:

```
Build type = debug:
$ ./za-cmake.sh debug && ./zb-build.sh debug all
```

Building just one of the Unit Tests can be done like this:

```
Build type = release, Build target = test_sysfnc:
$ ./za-cmake.sh && ./zb-build.sh test_sysfnc
```

Building the application as a stripped, statically linked executable can be done like this (`app` in this example is actually the default target):

```
Build type = release, Build target = app:
$ ./za-cmake.sh static strip && ./zb-build.sh release_stat_strip app
```

### Batch File for MS Windows

For convenience, there is a simple script for building all executables as stripped, statically linked executables on MS Windows:

```
> .\zb-build-all.cmd
```

## Unit Tests

### Bash Scripts for macOS / Linux

For convenience, there are two Bash scripts for running the Unit Tests on macOS / Linux.

To run a specific Unit Test, use the `zc-run-test.sh` script, e.g.:

```
Build type = release, Build target = test_sysfnc:
$ ./zc-run-test.sh sysfnc

Build type = debug, Build target = test_sysfnc:
$ ./zc-run-test.sh debug sysfnc

Build type = release, Build target = test_sysfnc, run test with Valgrind:
$ ./zc-run-test.sh vg_release_stat sysfnc

Build type = debug, Build target = test_sysfnc, run test with Valgrind:
$ ./zc-run-test.sh vg_debug_stat sysfnc
```

Some Unit Tests require additional parameters, e.g.:

```
Build type = release, Build target = test_dl:
$ ./zc-run-test.sh dl -- -s
```

To run all Unit Tests, e.g.:

```
Build type = release:
$ ./zc-run-test-all.sh

Build type = debug:
$ ./zc-run-test-all.sh debug

Build type = release, run test with Valgrind:
$ ./zc-run-test-all.sh vg_release_stat

Build type = debug, run test with Valgrind:
$ ./zc-run-test-all.sh vg_debug_stat
```

### Batch File for MS Windows

For convenience, there is a simple script for running all Unit Tests on MS Windows:

```
> .\zc-run-test-all.cmd
```
