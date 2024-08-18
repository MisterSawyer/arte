set(MODULE_SHORT_NAME type)
set(MODULE_NAME "${PROJECT_NAME}-${MODULE_SHORT_NAME}")

message(STATUS "Configuring ${PROJECT_NAME}::${MODULE_SHORT_NAME} module")

add_library("${MODULE_NAME}" OBJECT type.h)
add_library("${PROJECT_NAME}::${MODULE_SHORT_NAME}" ALIAS "${MODULE_NAME}")
set(PUBLIC_HEADERS_LIST
    "type.h"
)
target_sources("${MODULE_NAME}"
PRIVATE
    "src/type.cpp"
)
set_target_properties("${MODULE_NAME}" PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS_LIST}")
set_target_properties("${MODULE_NAME}" PROPERTIES CXX_STANDARD 23)

target_include_directories("${MODULE_NAME}" 
PUBLIC 
    "${MODULES_DIR}"
)

target_link_libraries("${MODULE_NAME}" 
PUBLIC
PRIVATE 
)
