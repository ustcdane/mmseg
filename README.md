mmseg
=====

MMSEG simple word segmenter in C++ 11 

  * Based on https://github.com/jdeng/mmseg
  * Data files from mmseg4j https://code.google.com/p/mmseg4j/
  * Compile ,In main.cpp
  * If use src/Mmseg.hpp,
  * g++ -Ofast -march=native -funroll-loops -o mmseg -std=c++11 main.cpp 
  * If use src/Mmseg.h
  * g++ -Ofast -march=native -funroll-loops -o mmseg -std=c++11  main.cpp src/Mmseg.cpp 

see main.cpp

Result:</br>
![mmseg测试 By Daniel](http://img.blog.csdn.net/20160110195458531)

Details:</br>
http://blog.csdn.net/daniel_ustc/article/details/50488040
