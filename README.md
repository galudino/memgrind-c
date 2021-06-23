# memgrind-c
 A benchmarking application for the `cgcs_malloc` memory allocator function.<br>
 (see https://github.com/galudino/cgcs_malloc for the `malloc`/`free` implementation)

## Building/running:

Run the included `makebuilds` script to have `cmake`<br>
create Unix-Makefile builds in the following modes:
- `Debug`
- `Release`
- `MinSizeRel`
- `RelWithDebInfo`

```
% ./makebuilds
```

A `build/make` subdirectory will be created with subdirectories<br>
of the modes described above. 

If we want to create a `Debug` build
of the `memgrind-c` program (which will also build the libraries in `./libs`):

```
% make -C ./build/make/Debug/src
```

Generally,
```
% make -C ./build/make/[build-mode]/[target-name]
```

You can then run the `memgrind-c` executable by invoking the following:
```
% ./build/make/Debug/src/memgrind-c
```

Building in `Release` mode will yield the shortest benchmarks.

### Alternate build systems

If you want to use an alternative build system, i.e. Xcode or Visual Studio<br>
(see the list of supported generators on your system using `cmake -help`), invoke the following:
```
% cmake -S ./ -B ./build/[generator-name] -G "[generator-name]"
```

For example, for Xcode:
```
% cmake -S ./ -B ./build/xcode -G "Xcode"
```

