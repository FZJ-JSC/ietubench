#ifndef MICRO_HPP
#define MICRO_HPP

#define _STR(x) #x
#define TO_STR(x) _STR(x)

#if defined(KERNEL_DRIVER_BP)
#define KERNEL_DRIVER DriverBP
#include "micro/bp.hpp"
#elif defined(KERNEL_DRIVER_C2C)
#define KERNEL_DRIVER DriverC2C
#include "micro/c2c.hpp"
#elif defined(KERNEL_DRIVER_IET)
#define KERNEL_DRIVER DriverIET
#include "micro/iet.hpp"
#elif defined(KERNEL_DRIVER_Lat)
#define KERNEL_DRIVER DriverLat
#include "micro/lat.hpp"
#else
#define KERNEL_DRIVER DriverIET
#include "micro/iet.hpp"
#endif

#endif // MICRO_HPP