# sessdsa2020_cpp_homework
homework solution for https://github.com/pkulab409/sessdsa2020 in C++

## Build
N.B. Require OpenMP >= 3.1, therefore unable to build with VS
### Default
```
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
```
### On Windows with MinGW
```
cmake -DCMAKE_BUILD_TYPE=Release . -G "MinGW Makefiles"
cmake --build .
```
## Run
Put the `Film.json` file in the same directory with the executable file. Then run the executable file.