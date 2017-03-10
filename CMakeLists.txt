cmake_minimum_required(VERSION 3.6)
project(Searcher)


set(CMAKE_CXX_STANDARD 11)

set(
        SOURCE_FILES main.cpp
        Video_controller.cpp
        Video_controller.h
        My_exeptions.cpp
        My_exeptions.h
        Shape_Finder.cpp Shape_Finder.h)

find_package( OpenCV 3 REQUIRED )

add_executable(Searcher ${SOURCE_FILES})

target_link_libraries( Searcher ${OpenCV_LIBS} )
target_link_libraries(Searcher pthread)
