cmake_minimum_required(VERSION 3.17)
project(SysPro/Project_1)

set(CMAKE_CXX_STANDARD 14)

add_executable(App main.cpp CitizenRecords.cpp CitizenRecords.h BloomFilter.cpp BloomFilter.h HashUtil.cpp HashUtil.h SkipList.cpp SkipList.h InputHandler.cpp InputHandler.h)
