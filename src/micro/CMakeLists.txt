# Define kernel build configuration
function(CONFIGURE_KERNEL)
    set(single_args DRIVER FLAGS)
    set(multi_args LOOP_LENGTHS PARALLELISM)
    cmake_parse_arguments(CONFIGURE_KERNEL "" "${single_args}" "${multi_args}" ${ARGN})

    set(DRIVER_SRCS
            ${CMAKE_SOURCE_DIR}/src/driver/micro.cpp
            ${CMAKE_SOURCE_DIR}/src/driver/cpu_freq.cpp
    )
    cmake_path(SET KERNEL_LIST "${CMAKE_CURRENT_SOURCE_DIR}/index.json")
    file(READ ${KERNEL_LIST} KERNELS_JSON)
    string(JSON KERNEL_NAMES_LENGTH  LENGTH ${KERNELS_JSON} kernels)
    math(  EXPR KERNEL_NAMES_LENGTH "${KERNEL_NAMES_LENGTH} - 1")

    foreach(IDX RANGE ${KERNEL_NAMES_LENGTH})
        string(JSON KERNEL_NAME GET ${KERNELS_JSON} kernels ${IDX} name)
        list(APPEND KERNEL_NAMES ${KERNEL_NAME})
    endforeach()

    if(NOT DEFINED CONFIGURE_KERNEL_PARALLELISM)
        set(CONFIGURE_KERNEL_PARALLELISM "0")
    endif()

    foreach(P IN LISTS CONFIGURE_KERNEL_PARALLELISM)
    foreach(L IN LISTS CONFIGURE_KERNEL_LOOP_LENGTHS)
        if ("${P}" STREQUAL "0")
            set(K_SUFFIX "${L}")
        else()
            set(K_SUFFIX "${L}_${P}")
        endif()
        set(KERNEL_SRCS)
        foreach(K IN LISTS KERNEL_NAMES)
            list(APPEND KERNEL_SRCS "${K}_${K_SUFFIX}.sx")
        endforeach()
        add_custom_command(
            OUTPUT ${KERNEL_SRCS}
            DEPENDS j2gen
            COMMAND j2gen -l ${L} -p ${P} -s _${K_SUFFIX}.sx -c ${KERNEL_LIST} -o ${CMAKE_CURRENT_BINARY_DIR}
        )
        foreach(K IN LISTS KERNEL_NAMES)
            add_executable(${K}_${K_SUFFIX}.x ${K}_${K_SUFFIX}.sx ${DRIVER_SRCS} )
            target_include_directories(${K}_${K_SUFFIX}.x PRIVATE ${CMAKE_SOURCE_DIR}/src/driver)
            target_compile_definitions(${K}_${K_SUFFIX}.x PRIVATE "KERNEL=${K}" "KERNEL_LOOP_LEN=${L}" "KERNEL_PARALLEL=${P}" "KERNEL_DRIVER=Driver${CONFIGURE_KERNEL_DRIVER}")
            target_compile_options(${K}_${K_SUFFIX}.x  PRIVATE  $<$<COMPILE_LANGUAGE:ASM>: ${CONFIGURE_KERNEL_FLAGS} ${ASM_FLAGS}> )
            target_link_libraries( ${K}_${K_SUFFIX}.x  PRIVATE Threads::Threads)
            set_property(SOURCE ${K}_${K_SUFFIX}.sx  PROPERTY LANGUAGE ASM)

            if(PROJECT_USE_M5)
                set_target_properties(${K}_${K_SUFFIX}.x PROPERTIES OUTPUT_NAME "${K}_${K_SUFFIX}-m5.x")
                target_link_libraries(${K}_${K_SUFFIX}.x PRIVATE m5)
                target_compile_definitions(${K}_${K_SUFFIX}.x PRIVATE USEM5OPS)
            endif()

            if(PROJECT_USE_LIKWID)
                set_target_properties(${K}_${K_SUFFIX}.x PROPERTIES OUTPUT_NAME "${K}_${K_SUFFIX}-lk.x")
                target_link_libraries(${K}_${K_SUFFIX}.x PRIVATE likwid)
                target_compile_definitions(${K}_${K_SUFFIX}.x PRIVATE LIKWID_PERFMON)
            endif()
        endforeach()
    endforeach()
    endforeach()
endfunction()

add_subdirectory(bp)
add_subdirectory(c2c)
add_subdirectory(iet)
add_subdirectory(sve)