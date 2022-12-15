CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

option(ENABLE_SUPERVISOR "Enable supervisor" ON)
if (WIN32)
  set(ENABLE_SUPERVISOR OFF CACHE INTERNAL "")
endif()

add_definitions( -DZF_LOG_LEVEL=ZF_LOG_DEBUG)

macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

use_cxx11()

FUNCTION(ADD_SUBDIRECTORY_BINDY)
    SET(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    SET(BUILD_EXAMPLE OFF CACHE INTERNAL "")
	SET(USE_JOINT_ZFLOG_FROM_ROOT_PROJECT ON CACHE INTERNAL "")
    ADD_SUBDIRECTORY(vendor/bindy)
ENDFUNCTION()
ADD_SUBDIRECTORY_BINDY()
INCLUDE_DIRECTORIES(vendor)

PROJECT(urpc_xinet_server CXX)
IF(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()
FUNCTION(ADD_SUBDIRECTORY_LIBURPC)
   ADD_SUBDIRECTORY(.. liburpc)
ENDFUNCTION()
ADD_SUBDIRECTORY_LIBURPC()
set(SOURCES 
    common.cpp
    server.cpp
    mapserialurpc.cpp)
if (ENABLE_SUPERVISOR)
    set(SOURCES ${SOURCES} supervisor.cpp)
endif()
if (WIN32)
    set(SOURCES ${SOURCES} platform-win32.cpp)
else()
    set(SOURCES ${SOURCES} platform-posix.cpp)
endif()
ADD_EXECUTABLE(urpc_xinet_server ${SOURCES})

TARGET_LINK_LIBRARIES(urpc_xinet_server zf_log urpc bindy)

IF (MSVC)
    add_custom_command(TARGET  urpc_xinet_server POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       "${LIBSERIALPORT_PATH}/libserialport.dll"
                       $<TARGET_FILE_DIR:urpc_xinet_server>)
ENDIF()
  