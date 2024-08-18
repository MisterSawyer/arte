set(MODULE_SHORT_NAME python)
set(MODULE_NAME "${PROJECT_NAME}-${MODULE_SHORT_NAME}")

message(STATUS "Configuring ${PROJECT_NAME}::${MODULE_SHORT_NAME} module")

# initialize Python
message(STATUS "[${PROJECT_NAME}] initializing `Python` ...")
set(Python3_USE_STATIC_LIBS FALSE)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
message(STATUS "initialized `python` from ${Python3_STDARCH}, include ${Python3_INCLUDE_DIRS}, libs ${Python3_LIBRARY_DIRS}")

add_library("${MODULE_NAME}" OBJECT python.h)
add_library("${PROJECT_NAME}::${MODULE_SHORT_NAME}" ALIAS "${MODULE_NAME}")
set(PUBLIC_HEADERS_LIST
    "python.h"
)
target_sources("${MODULE_NAME}"
PRIVATE
    "src/python.cpp"
)
set_target_properties("${MODULE_NAME}" PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS_LIST}")
set_target_properties("${MODULE_NAME}" PROPERTIES CXX_STANDARD 23)

target_include_directories("${MODULE_NAME}" 
PUBLIC 
    "${MODULES_DIR}"
    "${Python3_INCLUDE_DIRS}"
    "${asio_SOURCE_DIR}/include"
)

target_link_libraries("${MODULE_NAME}" 
PUBLIC
    OpenSSL::SSL
    OpenSSL::Crypto
    Python3::Python

PRIVATE 
    "${PROJECT_NAME}::type"
    "${PROJECT_NAME}::transaction"
)

message(STATUS "[${MODULE_NAME}] adding install target")
install(TARGETS "${MODULE_NAME}" EXPORT "${MODULE_NAME}Targets"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"                                   # bin
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"                                   # lib
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"                                   # lib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PUBLIC_HEADERS_DIR}"   # include/PUBLIC_HEADERS_DIR
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"                              # include
)
