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
        schedulers/SchedulersController.cpp schedulers/SchedulersController.h schedulers/TemperatureScheduler.cpp schedulers/TemperatureScheduler.h schedulers/WindScheduler.cpp schedulers/WindScheduler.h
        core/Cell.cpp core/Cell.h
        core/CellType.cpp core/CellType.h
        core/Configurations.h
        core/EditItem.h
        core/OilComponent.cpp core/OilComponent.h
        core/OilPoint.cpp core/OilPoint.h
        core/Sea.cpp core/Sea.h
        core/Vector2.cpp core/Vector2.h exceptions.h components/EmulsificationComponent.cpp components/EmulsificationComponent.h components/EvaporationComponent.cpp components/EvaporationComponent.h components/TemperatureDependency.cpp components/TemperatureDependency.h components/ViscosityUpdateComponent.cpp components/ViscosityUpdateComponent.h
        systems/ChangeSquareSystem.cpp systems/ChangeSquareSystem.h systems/OilSystem.h systems/TimeCounter.cpp systems/TimeCounter.h systems/OilPointComponentSystem.cpp systems/OilPointComponentSystem.h systems/ReEntairedSystem.cpp systems/ReEntairedSystem.h systems/SPreadingSystem.cpp systems/SPreadingSystem.h statistics/Statistics.cpp statistics/Statistics.h statistics/StatisticsType.cpp statistics/StatisticsType.h)


message("Root dir: " ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR})