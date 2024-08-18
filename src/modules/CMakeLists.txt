add_subdirectory(native)
add_subdirectory(python)
add_subdirectory(type)
add_subdirectory(transaction)
add_subdirectory(client)
add_subdirectory(server)

target_link_libraries("${PROJECT_NAME}"
    PRIVATE
    "${PROJECT_NAME}::python"
    "${PROJECT_NAME}::type"
    "${PROJECT_NAME}::transaction"
    "${PROJECT_NAME}::client"
    "${PROJECT_NAME}::server"

    # requred to be last
    "${PROJECT_NAME}::native"
)