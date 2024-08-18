cmake_minimum_required(VERSION 3.22)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0135 NEW)
set(CMAKE_INSTALL_MESSAGE LAZY)

include(FetchContent)
FetchContent_Declare(
  asio
  URL      https://sourceforge.net/projects/asio/files/asio/1.30.2%20%28Stable%29/asio-1.30.2.tar.gz/download
)

# --------------------------------------------
# arte project
# --------------------------------------------
set(PROJECT_NAME "arte")
project("${PROJECT_NAME}"         
          VERSION "1.0"
          DESCRIPTION "arte proof of concept"
          HOMEPAGE_URL "https://github"
          LANGUAGES CXX
)

# initialize asio
message(STATUS "[${PROJECT_NAME}] initializing `asio` ...")
FetchContent_MakeAvailable(asio)
message(STATUS "initialized `asio` from ${asio_SOURCE_DIR} to ${asio_BINARY_DIR}")

#initialize openssl
find_package(OpenSSL REQUIRED)

# initialize arte
set(PUBLIC_HEADERS_LIST "include/arte.h")
add_executable("${PROJECT_NAME}" "${PROJECT_SOURCE_DIR}/src/main.cpp")

set_target_properties("${PROJECT_NAME}" PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS_LIST}")
set_target_properties("${PROJECT_NAME}" PROPERTIES CXX_STANDARD 23)
set_target_properties("${PROJECT_NAME}" PROPERTIES DEBUG_POSTFIX "_D")

set(MODULES_DIR "${PROJECT_SOURCE_DIR}/src/modules")
add_subdirectory("${MODULES_DIR}")

target_include_directories("${PROJECT_NAME}" 
  PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    "${MODULES_DIR}"
    "${asio_SOURCE_DIR}/include"
  PRIVATE
)

target_link_libraries("${PROJECT_NAME}"
  PUBLIC

  PRIVATE
)

if(DEFINED CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(STATUS
  "[${PROJECT_NAME}] "
  "CMAKE_INSTALL_PREFIX is not set\n\t"
  "Default value: ${CMAKE_INSTALL_PREFIX}\n\t"
  "Will set it to ${CMAKE_SOURCE_DIR}/${DEFAULT_DIR}")
  set(CMAKE_INSTALL_PREFIX
  "${CMAKE_SOURCE_DIR}/${DEFAULT_DIR}"
  CACHE PATH "Where the library will be installed to" FORCE)
else()
  message(STATUS
  "[${PROJECT_NAME}] "
  "CMAKE_INSTALL_PREFIX was already set\n\t"
  "Current value: ${CMAKE_INSTALL_PREFIX}")
endif()

if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR "include" CACHE PATH "C header files (include)")
endif()

message(STATUS "[${PROJECT_NAME}] adding install target")
install(TARGETS "${PROJECT_NAME}" EXPORT "${PROJECT_NAME}Targets"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"                                   # bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"                                   # lib
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"                                   # lib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PUBLIC_HEADERS_DIR}"   # include/PUBLIC_HEADERS_DIR
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"                              # include
)