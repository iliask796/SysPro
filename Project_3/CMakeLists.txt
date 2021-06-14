cmake_minimum_required(VERSION 3.17)
project(SysPro/Project_3)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(App mainClient.cpp CitizenRecords.cpp CitizenRecords.h HashUtil.cpp HashUtil.h BloomFilter.cpp BloomFilter.h InputHandler.cpp InputHandler.h TravelRecords.cpp TravelRecords.h)
add_executable(monitorServer Server.cpp CitizenRecords.cpp CitizenRecords.h HashUtil.cpp HashUtil.h BloomFilter.cpp BloomFilter.h SkipList.cpp SkipList.h InputHandler.cpp InputHandler.h TravelRecords.cpp TravelRecords.h)