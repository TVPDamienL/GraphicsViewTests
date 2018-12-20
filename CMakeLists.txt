CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
CMAKE_POLICY(SET CMP0054 NEW)

# Project name
project(GraphicsView C CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

#basic conf
IF( WIN32 )
    SET( WINDOWS 1 )
ELSEIF( APPLE )
    SET(MACOSX 1)
ELSEIF( UNIX )
    SET(LINUX 1)
ELSEIF( MINGW )
    SET(WINDOWS 1)
ELSEIF( CYGWIN )
    SET(WINDOWS 1)
ELSE()
    MESSAGE("WHAT IS THIS PLATFORM ?")
    RETURN()
ENDIF()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(INCLUDE_DIRECTORIES "")
set(LINK_LIBS "")

if( LINUX )
set(GraphicsView_DEFINITIONS LINUX)
elseif(WINDOWS)
set(GraphicsView_DEFINITIONS WINDOWS)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets 5.10)
find_package(Qt5Core 5.10)

##################################################

function(extract_include_dirs header_files result_varname)
set(header_directories "")
foreach(header in ${header_files})
get_filename_component(tmp ${header} DIRECTORY)
list( APPEND header_directories ${tmp})
endforeach(header in ${header_files})
list(REMOVE_DUPLICATES header_directories)

set(${result_varname} ${header_directories} PARENT_SCOPE)
endfunction(extract_include_dirs)


file(GLOB_RECURSE GraphicsView_SRC_FILES RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/GraphicsViewTests/*.cpp")
file(GLOB_RECURSE GraphicsView_HEADER_FILES RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/GraphicsViewTests/*.h*")

extract_include_dirs("${GraphicsView_HEADER_FILES}" GraphicsView_INCLUDE_DIRECTORIES)

set( GraphicsView_INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${GraphicsView_INCLUDE_DIRECTORIES})

set_source_files_properties(GraphicsView_HEADER_FILES PROPERTIES HEADER_FILE_ONLY TRUE)
add_executable(${PROJECT_NAME} ${GraphicsView_SRC_FILES} ${GraphicsView_HEADER_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${GraphicsView_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${GraphicsView_INCLUDE_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets stdc++fs)
