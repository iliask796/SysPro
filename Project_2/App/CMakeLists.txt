cmake_minimum_required(VERSION 3.17)
project(SysPro/Project_2)

set(CMAKE_CXX_STANDARD 14)

add_executable(App main.cpp CitizenRecords.cpp CitizenRecords.h HashUtil.cpp HashUtil.h BloomFilter.cpp BloomFilter.h InputHandler.cpp InputHandler.h)
add_executable(Monitor Monitor.cpp CitizenRecords.cpp CitizenRecords.h HashUtil.cpp HashUtil.h BloomFilter.cpp BloomFilter.h SkipList.cpp SkipList.h InputHandler.cpp InputHandler.h)