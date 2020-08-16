cmake_minimum_required(VERSION 3.13)
project(OilSpill)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 11)

if (MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /FS")
endif ()

project(OilSpill)
add_executable(OilSpill main.cpp schedulers/CurrentScheduler.cpp schedulers/CurrentScheduler.h
        schedulers/Scheduler.cpp schedulers/Scheduler.h schedulers/OilScheduler.cpp schedulers/OilScheduler.h
        schedulers/SchedulersController.cpp schedulers/SchedulersController.h schedulers/TemperatureScheduler.cpp schedulers/TemperatureScheduler.h schedulers/WindScheduler.cpp schedulers/WindScheduler.h)