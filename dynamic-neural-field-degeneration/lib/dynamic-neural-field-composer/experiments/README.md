# Experiments using the composer

Experiments are designed to be a standalone executable file that runs independently from the ```dynamic-neural-field-composer.cpp``` executable.

You can either run an experiment OR the normal application.

## How to use

1. Create a .cpp file;
2. Set it as the launchable .exe in the CMakeLists.txt file.

```
add_executable(launch-dynamic-neural-field-composer 
    #"src/dynamic-neural-field-composer.cpp"
    "experiments/degeneracy.cpp"
)
```