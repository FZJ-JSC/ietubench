
add_executable(j2gen)

target_sources(j2gen PRIVATE main.cpp)

include(FetchContent)

FetchContent_Declare(inja
  URL      https://github.com/pantor/inja/releases/download/v3.4.0/inja.hpp
  URL_HASH MD5=78459ecf0731fec32451871683d36b2a
  DOWNLOAD_NO_EXTRACT true
)
FetchContent_MakeAvailable(inja)

target_include_directories(j2gen PRIVATE ${inja_SOURCE_DIR})
