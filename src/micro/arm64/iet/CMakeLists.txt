# Use IET driver
configure_kernel(DRIVER IET FLAGS "-march=armv8.2-a" LOOP_LENGTHS ${PROJECT_LOOP_LENGTHS})
