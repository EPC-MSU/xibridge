CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(xibridge)
OPTION(BUILD_SHARED_LIBS_XI "Build shared libraries" ON)
OPTION(BUILD_TEST "Build xibridge test project" OFF)
OPTION(BUILD_XIEXAMPLE "Build xibridge example@ project" OFF)

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

#to build test and xiexample in bin/folder and avoid ld errors
IF (NOT MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
ENDIF()  

IF(${CMAKE_SYSTEM_NAME} STREQUAL Windows AND ${CMAKE_SYSTEM_VERSION} EQUAL 6.1)
    # HACK: Make cryptopp replace missing (till Windows 8) GetOverlappedResultEx() on Windows7 with somehting compatible
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0601)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD OR ${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
ENDIF()


CONFIGURE_FILE(config_xi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config_xi.h)
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

SET(SOURCES
        client/xibridge_client.cpp
		client/bindy_helper.cpp
        common/protocols.cpp
        common/utils.cpp
		client/xibridge.cpp
		client/version.rc
         )

#add_dependencies
if (BUILD_SHARED_LIBS_XI)
  ADD_LIBRARY(xibridge SHARED ${SOURCES})
else()
  ADD_LIBRARY(xibridge STATIC ${SOURCES})
endif()
if(MSVC)
  target_compile_options(xibridge PRIVATE /W3 /WX)
else()
  target_compile_options(xibridge PRIVATE -Wall -Wextra -Werror)
endif()
TARGET_LINK_LIBRARIES(xibridge bindy zf_log)

#add_custom_target(tests) # Note: without 'ALL'
IF(BUILD_TEST) 
  ADD_EXECUTABLE(test  test/test.cpp test/test_proto.cpp ${SOURCES}) # Note: without 'ALL'
  if(MSVC)
     target_compile_options(test PRIVATE /W3 /WX)
  else()
     if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
        target_compile_options(test PRIVATE -Wall -Wextra -Werror)
   endif()
  endif()
  TARGET_LINK_LIBRARIES(test bindy zf_log)
ENDIF() 

IF(BUILD_XIEXAMPLE)
    ADD_EXECUTABLE(xiexample  xiexample/xiexample.cpp xiexample/xinet_server_test_1.cpp xiexample/xinet_server_test_2.cpp)
    TARGET_LINK_LIBRARIES(xiexample xibridge zf_log)
    if(MSVC)
     target_compile_options(xiexample PRIVATE /W3 /WX)
    else()
     if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8)
        target_compile_options(xiexample PRIVATE -Wall -Wextra -Werror)
     endif()
    endif()	 
ENDIF()


