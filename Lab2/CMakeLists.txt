# Top-Level CMakeList.txt

cmake_minimum_required (VERSION 3.7)
project ( SecondLab )

set (CMAKE_CXX_STANDARD 14)

add_executable( SecondLab Source.cpp)
target_include_directories(SecondLab PRIVATE ../Lab1/glm)
target_include_directories(SecondLab PRIVATE ../Lab1/)

find_package (SDL)

if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

if(SDL_FOUND)
    include_directories(
            ${SDL_INCLUDE_DIR}
            ${PROJECT_SOURCE_DIR}/glm
    )
    #link_libraries(${SDL_LIBRARY})
    target_link_libraries(SecondLab ${SDL_LIBRARY})
endif(SDL_FOUND)

