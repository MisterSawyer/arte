set(MODULE_SHORT_NAME server)
set(MODULE_NAME "${PROJECT_NAME}-${MODULE_SHORT_NAME}")

message(STATUS "Configuring ${PROJECT_NAME}::${MODULE_SHORT_NAME} module")

add_library("${MODULE_NAME}" OBJECT server.h)
add_library("${PROJECT_NAME}::${MODULE_SHORT_NAME}" ALIAS "${MODULE_NAME}")
set(PUBLIC_HEADERS_LIST
    "server.h"
)
target_sources("${MODULE_NAME}"
PRIVATE
    "src/server.cpp"
)
set_target_properties("${MODULE_NAME}" PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS_LIST}")
set_target_properties("${MODULE_NAME}" PROPERTIES CXX_STANDARD 23)

target_include_directories("${MODULE_NAME}" 
PUBLIC 
    "${MODULES_DIR}"
    "${asio_SOURCE_DIR}/include"
)

target_link_libraries("${MODULE_NAME}" 
PUBLIC
PRIVATE 
)
