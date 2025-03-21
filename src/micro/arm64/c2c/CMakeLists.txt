# Use C2C driver
configure_kernel(DRIVER C2C FLAGS "-march=armv8.2-a" LOOP_LENGTHS ${PROJECT_LOOP_LENGTHS})
