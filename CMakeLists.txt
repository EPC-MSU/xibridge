CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(xibridge)
OPTION(BUILD_SHARED_LIBS_XI "Build shared libraries" ON)

CONFIGURE_FILE(config_xi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config_xi.h)
add_definitions( -DZF_LOG_LEVEL=ZF_LOG_DEBUG)
if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
   add_definitions( -D_CRT_SECURE_NO_WARNINGS) 
endif()
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
	 
#add_custom_target(tests) # Note: without 'ALL'
IF(NOT HAS_PARENT) 
  ADD_EXECUTABLE(test  test/test_main.cpp test/test_proto.cpp test/xinet_server_test_1.cpp test/xinet_server_test_2.cpp ${SOURCES}) # Note: without 'ALL'
  TARGET_LINK_LIBRARIES(test bindy zf_log)
ENDIF()  
#add_dependencies(tests test_executable)
if (BUILD_SHARED_LIBS_XI)
  ADD_LIBRARY(xibridge SHARED ${SOURCES})
else()
  ADD_LIBRARY(xibridge STATIC ${SOURCES})
endif()
TARGET_LINK_LIBRARIES(xibridge bindy zf_log)

