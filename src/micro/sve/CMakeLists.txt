# Use IET driver
configure_kernel(DRIVER IET FLAGS "-march=armv8.2-a+sve" LOOP_LENGTHS ${PROJECT_LOOP_LENGTHS})
