cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME H_Tracker)
project(${PROJECT_NAME})

set(source_dir ${PROJECT_SOURCE_DIR}/src)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.3/5.12.3/msvc2017_64")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-DQT_DEPRECATED_WARNINGS -DNOMINMAX -D_HAS_CXX17=1)

# find all .h and .cpp files:
file(GLOB_RECURSE source_files ${source_dir}/*.cpp)
file(GLOB_RECURSE include_files ${source_dir}/*.h)

# find all subdirectories to include
file(GLOB_RECURSE all_files_folders LIST_DIRECTORIES true ${source_dir}/*)
SET(include_dirs "")
FOREACH(file_or_folder ${all_files_folders})
    IF(IS_DIRECTORY ${file_or_folder})
        LIST(APPEND include_dirs ${file_or_folder})
    ENDIF()
ENDFOREACH()

include_directories(${PROJECT_NAME}
        ${source_dir}
        ${include_dirs}
        "C:/dev/eigen")

add_executable(${PROJECT_NAME} ${source_files} ${include_files})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Network)
target_link_libraries(${PROJECT_NAME} Qt5::Gui)
target_link_libraries(${PROJECT_NAME} Qt5::PrintSupport)


