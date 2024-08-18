set(MODULE_SHORT_NAME native)
set(MODULE_NAME "${PROJECT_NAME}-${MODULE_SHORT_NAME}")

message(STATUS "Configuring ${PROJECT_NAME}::${MODULE_SHORT_NAME} module")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "[${MODULE_NAME}] configuring for Windows")
    add_library("${MODULE_NAME}" OBJECT "windows/native.h")
    set(PUBLIC_HEADERS_LIST
        "windows/native.h"
    )
    target_sources("${MODULE_NAME}"
    PRIVATE
        "src/windows.cpp"
    )
    target_include_directories("${MODULE_NAME}" PUBLIC "windows")
    target_link_libraries("${MODULE_NAME}" PUBLIC ws2_32 wsock32)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(STATUS "[${MODULE_NAME}] configuring for Linux")
  add_library("${MODULE_NAME}" OBJECT "linux/native.h")
  set(PUBLIC_HEADERS_LIST
      "linux/native.h"
  )
  target_sources("${MODULE_NAME}"
  PRIVATE
      "src/linux.cpp"
  )
  target_include_directories("${MODULE_NAME}" PUBLIC "linux")
  set(ZLIB_USE_STATIC_LIBS "ON")
  target_link_libraries("${MODULE_NAME}" PUBLIC expat z)
endif()

add_library("${PROJECT_NAME}::${MODULE_SHORT_NAME}" ALIAS "${MODULE_NAME}")

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
