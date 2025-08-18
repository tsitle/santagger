# Cross-Compiling Docker Images

The Docker Images are based on [dockcross](https://github.com/dockcross/dockcross).

They additionally contain cross-compiled versions of:  
- libmpg123
- libogg
- libvorbis
- libz

Only x86_64/amd64 hosts are supported.  
Supported targets:  
- Linux x86_64 (aka amd64)
- Linux aarch64 (aka arm64)
- MS Windows x86_64 (aka amd64)
- MS Windows aarch64 (aka arm64)

## Building the Docker Images

```
$ ./build_images.sh
```

or if you want to build only one image, e.g.:

```
$ ./build_images.sh linux-arm64
```

This will also create the `run-st_dockcross-*.sh` scripts.

## Using the Docker Images

The run scripts will mount the current working directory to `~/work/`.

Usage examples:

```
$ ./run-st_dockcross-linux_arm64.sh bash
```

```
$ ./run-st_dockcross-linux_arm64.sh qemu-aarch64-static some_linux_aarch64_binary --arg value
```

```
$ ./run-st_dockcross-windows_x64.sh sudo wine some_windows_x64_binary.exe --arg value
```

Unfortunately, the MS Windows AARCH64 image does contain neither qemu nor wine.
