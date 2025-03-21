# Use Lat driver
configure_kernel(DRIVER Lat FLAGS "-march=armv8.2-a" LOOP_LENGTHS "4096" PARALLELISM ${PROJECT_K_PARALLELISM})
