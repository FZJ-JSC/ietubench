add_executable(random_data)

target_sources(random_data PRIVATE random_data.cpp)

add_custom_command(
    TARGET random_data
    POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/random_data "409600" "100"
    BYPRODUCTS "data_s409600_r100.dat"
    VERBATIM
)
