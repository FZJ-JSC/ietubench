function(CONFIGURE_KERNELS DRIVER_NAME KERNEL_FLAGS)
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

    foreach(L IN LISTS PROJECT_LOOP_LENGTHS)
        set(KERNEL_SRCS)
        foreach(K IN LISTS KERNEL_NAMES)
            list(APPEND KERNEL_SRCS "${K}_${L}.sx")
        endforeach()
        add_custom_command(
            OUTPUT ${KERNEL_SRCS}
            DEPENDS j2gen
            COMMAND j2gen -l ${L} -c ${KERNEL_LIST} -o ${CMAKE_CURRENT_BINARY_DIR}
        )
        foreach(K IN LISTS KERNEL_NAMES)
            add_executable(${K}_${L}.x ${K}_${L}.sx ${DRIVER_SRCS} )
            target_include_directories(${K}_${L}.x PRIVATE ${CMAKE_SOURCE_DIR}/src/driver)
            target_compile_definitions(${K}_${L}.x PRIVATE "KERNEL=${K}" "KERNEL_LOOPL=${L}" "KERNEL_DRIVER=Driver${DRIVER_NAME}")
            target_compile_options(${K}_${L}.x  PRIVATE  $<$<COMPILE_LANGUAGE:ASM>: ${KERNEL_FLAGS} ${ASM_FLAGS}> )
            target_link_libraries( ${K}_${L}.x  PRIVATE Threads::Threads)
            set_property(SOURCE ${K}_${L}.sx  PROPERTY LANGUAGE ASM)

            if(PROJECT_USE_M5)
                set_target_properties(${K}_${L}.x PROPERTIES OUTPUT_NAME "${K}_${L}-m5.x")
                target_link_libraries(${K}_${L}.x PRIVATE m5)
                target_compile_definitions(${K}_${L}.x PRIVATE USEM5OPS)
            endif()

            if(PROJECT_USE_LIKWID)
                set_target_properties(${K}_${L}.x PROPERTIES OUTPUT_NAME "${K}_${L}-lk.x")
                target_link_libraries(${K}_${L}.x PRIVATE likwid)
                target_compile_definitions(${K}_${L}.x PRIVATE LIKWID_PERFMON)
            endif()
        endforeach()
    endforeach()
endfunction()

add_subdirectory(bp)
add_subdirectory(c2c)
add_subdirectory(iet)
add_subdirectory(sve)