CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(xibridge)

OPTION(BUILD_TEST "Build xibridge test project" ON)
OPTION(BUILD_XIBRIDGE_EXAMPLE "Build xibridge example@ project" OFF)
OPTION(BUILD_XIBRIDGE_SERVER "Build xibridge server" OFF)
OPTION(XIBRIDGE_SERVER_ENABLE_SUPERVISOR "Enable supervisor when build xibridge server" OFF)

#enable supervisor only in case of Debian Linux mipsel
IF (NOT ${CMAKE_SYSTEM_NAME} STREQUAL Linux)
  set(XIBRIDGE_SERVER_ENABLE_SUPERVISOR  OFF CACHE INTERNAL "")
ELSEIF (NOT  ${CMAKE_SYSTEM_PROCESSOR} STREQUAL mips)
  set(XIBRIDGE_SERVER_ENABLE_SUPERVISOR  OFF CACHE INTERNAL "")
ENDIF()

#set the right version here !
SET (XIBRIDGE_VERSION_MAJOR 1)
SET (XIBRIDGE_VERSION_MINOR 1)
SET (XIBRIDGE_VERSION_PATCH 7)

SET (XIBRIDGE_SERVER_SUPERVISOR_LIMIT 30)

IF(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    # It is required to build a rpath framework
    CMAKE_POLICY(SET CMP0042 NEW)
ENDIF()

# Detect correct C++11 flag
IF(NOT ${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    INCLUDE(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    IF(COMPILER_SUPPORTS_CXX11)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    ELSEIF(COMPILER_SUPPORTS_CXX0X)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    ELSE()
        MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    ENDIF()
ENDIF()

#to build test and example in bin/folder and avoid ld errors
IF (NOT MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
ENDIF()  

set(CMAKE_RUNTIME_INC_DIRECTORY ${CMAKE_SOURCE_DIR}/inc)

IF(${CMAKE_SYSTEM_NAME} STREQUAL Windows AND ${CMAKE_SYSTEM_VERSION} EQUAL 6.1)
    # HACK: Make cryptopp replace missing (till Windows 8) GetOverlappedResultEx() on Windows7 with somehting compatible
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0601)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD OR ${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
ENDIF()

CONFIGURE_FILE(client/version.h.in ${CMAKE_RUNTIME_INC_DIRECTORY}/client/version.h)
CONFIGURE_FILE(client/version_config.rc.in ${CMAKE_RUNTIME_INC_DIRECTORY}/client/version.rc)
CONFIGURE_FILE(server/supervisor.h.in ${CMAKE_RUNTIME_INC_DIRECTORY}/server/supervisor.h)
  
add_definitions( -DZF_LOG_LEVEL=ZF_LOG_DEBUG)
if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
   add_definitions( -D_CRT_SECURE_NO_WARNINGS) 
endif()

# set fPIC for static and shared libs
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

FUNCTION(ADD_SUBDIRECTORY_BINDY)
    SET(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    SET(BUILD_EXAMPLE OFF CACHE INTERNAL "")
    SET(USE_JOINT_ZFLOG_FROM_ROOT_PROJECT ON CACHE INTERNAL "")
    ADD_SUBDIRECTORY(vendor/bindy)
ENDFUNCTION()
ADD_SUBDIRECTORY_BINDY()
INCLUDE_DIRECTORIES(vendor)

GET_DIRECTORY_PROPERTY(HAS_PARENT PARENT_DIRECTORY)
FUNCTION(ADD_SUBDIRECTORY_ZF_LOG)
IF(NOT HAS_PARENT) 
    SET(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    add_definitions(-DZF_LOG_EXTERN_GLOBAL_OUTPUT_LEVEL)
    ADD_SUBDIRECTORY(vendor/zf_log)
    remove_definitions(-DZF_LOG_EXTERN_GLOBAL_OUTPUT_LEVEL)
ENDIF() 
ENDFUNCTION()
ADD_SUBDIRECTORY_ZF_LOG()

SET (COMMON_SOURCES 
        common/protocols.cpp
        common/utils.cpp
    )

SET(SOURCES
        client/xibridge_client.cpp
        client/bindy_helper.cpp
        client/xibridge.cpp
        ${COMMON_SOURCES}
    )

ADD_LIBRARY(xibridge SHARED ${SOURCES} inc/client/version.rc)

if(MSVC)
  target_compile_options(xibridge PRIVATE /W3 /WX)
else()
  target_compile_options(xibridge PRIVATE -Wall -Wextra -Werror)
endif()
TARGET_LINK_LIBRARIES(xibridge bindy zf_log)

#to build server_simu needed in tests # Note: without 'ALL'
IF(BUILD_TEST) 
  ADD_EXECUTABLE(server_simu  test/server_simu/server_simu.cpp ${COMMON_SOURCES}) # Note: without 'ALL'
  if(MSVC)
     target_compile_options(server_simu PRIVATE /W3 /WX)
  else()
     if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
        target_compile_options(server_simu PRIVATE -Wall -Wextra -Werror)
   endif()
  endif()
  TARGET_LINK_LIBRARIES(server_simu bindy zf_log)
ENDIF() 

#add_custom_target(unittests) # Note: without 'ALL'
IF(BUILD_TEST) 
  ADD_EXECUTABLE(unittests  test/test.cpp ${SOURCES}) # Note: without 'ALL'
  if(MSVC)
     target_compile_options(unittests PRIVATE /W3 /WX)
  else()
     if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
        target_compile_options(unittests PRIVATE -Wall -Wextra -Werror)
   endif()
  endif()
  TARGET_LINK_LIBRARIES(unittests bindy zf_log)
ENDIF() 

IF(BUILD_XIBRIDGE_EXAMPLE)
    ADD_EXECUTABLE(example  example/example.cpp example/xinet_server_ximc.cpp example/xinet_server_urpc.cpp example/xinet_server_xibridge.cpp)
    TARGET_LINK_LIBRARIES(example xibridge zf_log)
    if(MSVC)
     target_compile_options(example PRIVATE /W3 /WX)
    else()
     if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
        target_compile_options(example PRIVATE -Wall -Wextra -Werror)
     endif()
    endif()  
ENDIF()

IF(BUILD_XIBRIDGE_SERVER)
   set(SRV_LIB_SOURCES 
        server/xibridge_server_lib.cpp
        server/mapdevidphnd.cpp
        server/devid2usb.cpp
        ${COMMON_SOURCES})
   if (XIBRIDGE_SERVER_ENABLE_SUPERVISOR )
        set(SRV_LIB_SOURCES ${SRV_LIB_SOURCES} server/supervisor.cpp)
   endif()
   if (WIN32)
        set(SRV_LIB_SOURCES ${SRV_LIB_SOURCES} server/platform-win32.cpp)
   else()
        set(SRV_LIB_SOURCES ${SRV_LIB_SOURCES} server/platform-posix.cpp)
       if (${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
           set(SRV_LIB_SOURCES ${SRV_LIB_SOURCES} server/platform-macos.cpp)
       else()
           set(SRV_LIB_SOURCES ${SRV_LIB_SOURCES} server/platform-linux.cpp)
       endif()
   endif()
   ADD_LIBRARY(xibridge_server_lib SHARED ${SRV_LIB_SOURCES})

   FUNCTION(ADD_SUBDIRECTORY_XIB_SERIAL)
       ADD_SUBDIRECTORY(server/xib-serial)
   ENDFUNCTION()
   ADD_SUBDIRECTORY_XIB_SERIAL()
   include_directories(${LIBSERIALPORT_INCLUDE_DIR})

   include_directories(server)
   TARGET_LINK_LIBRARIES(xibridge_server_lib zf_log xib-serial bindy)
   IF(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
       TARGET_LINK_LIBRARIES(xibridge_server_lib LINK_PRIVATE setupapi)
   endif()

   INCLUDE_DIRECTORIES(vendor/zf_log/zf_log)
   ADD_EXECUTABLE(xibridge_server server/xibridge_server.cpp)
   TARGET_LINK_LIBRARIES(xibridge_server xibridge_server_lib)
   IF (MSVC)
        add_custom_command(TARGET  xibridge_server POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       "${LIBSERIALPORT_PATH}/libserialport.dll"
                       $<TARGET_FILE_DIR:xibridge_server>)
   ENDIF()
  
ENDIF()  