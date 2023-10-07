# Project F

F is a toy, functional, LISP-like language. It is built as a project for the
course on compiler construction at Innopolis University.

A general overview of the language is on Moodle. The list of this
implementation's details is available [here][implementation-details].

[implementation-details]: https://hackmd.io/@snejugal/B1D9F5QC3

## Building

First, initialize `cmake`.

```bash
cmake .
```

If you want to build with debug symbols, set `CMAKE_BUILD_TYPE` variable to
`Debug`. The variable value will be saved for all further local builds. 

```bash
cmake -DCMAKE_BUILD_TYPE=Debug .
```

After initialization, you can build the project.

```bash
cmake --build .
```

Now you can run `./project-f` to run the interpreter.
