cmake_minimum_required(VERSION 3.7)
project(libssh2-js
	DESCRIPTION "LibSSH2 Over Stream(WebSocket|WebRTC|Socket) in browsers or nodejs"
	VERSION 0.0.1
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	message(FATAL_ERROR "libssh2-js must be compiled with Emscripten.")
endif()

set(LIBSSH2_JS_PROG libssh2)

set(LIBSSH2_JS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIBSSH2_JS_SRC
	${LIBSSH2_JS_SRC_DIR}/ssh2-js.cpp
	${LIBSSH2_JS_SRC_DIR}/emsocket.cpp)

set(DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps")

set(LIBSSH2_JS_LIB  "${CMAKE_CURRENT_SOURCE_DIR}/js/libssh2-library.js")
set(LIBSSH2_WRAPPER_JS "${CMAKE_CURRENT_SOURCE_DIR}/js/libssh2-wrapper.js")

include_directories("${DEPS_DIR}/include")
link_directories("${DEPS_DIR}/lib")

set(WASM_OPTS "-s MODULARIZE --bind --js-library ${LIBSSH2_JS_LIB} --post-js ${LIBSSH2_WRAPPER_JS} -s EXPORT_NAME='ssh2Loader'")
set(DEPS_LIBS ssh2 mbedtls mbedcrypto z)

add_executable(${LIBSSH2_JS_PROG} ${LIBSSH2_JS_SRC})
set_target_properties(${LIBSSH2_JS_PROG} PROPERTIES LINK_FLAGS "${WASM_OPTS}")
target_link_libraries(${LIBSSH2_JS_PROG} ${DEPS_LIBS})
