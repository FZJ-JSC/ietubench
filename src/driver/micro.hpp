#ifndef MICRO_HPP
#define MICRO_HPP

#include "micro/bp.hpp"
#include "micro/c2c.hpp"
#include "micro/iet.hpp"

#define _STR(x) #x
#define TO_STR(x) _STR(x)

#ifndef KERNEL_DRIVER
#define KERNEL_DRIVER DriverIET
#endif

#endif // MICRO_HPP