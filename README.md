# sessdsa2020_cpp_homework
homework solution for https://github.com/pkulab409/sessdsa2020 in C++

经过一些优化后，此代码可以在Intel® i7-8565U CPU @ 1.80GHz的笔记本电脑上2分钟以内运算出结果。

## Build
N.B. Require OpenMP >= 3.1, therefore unable to build with Visual Studio
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