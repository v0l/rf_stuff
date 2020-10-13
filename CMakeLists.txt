cmake_minimum_required(VERSION 3.0.0)
project(rf_stuff)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ALL_INCLUDE_DIRS
    include
)

### SRC files for sdr lib
set(LIB_SDR_SRC
    src/sdr.cpp
    src/hackrf.cpp
)

## SRC files for UI lib
set(LIB_UI_SRC
    src/sdl2.cpp
    src/spectrogram.cpp
    src/fft.cpp
)

## radio_util_sdr lib
add_library(radio_util_sdr ${LIB_SDR_SRC})
target_include_directories(radio_util_sdr
    PUBLIC ${ALL_INCLUDE_DIRS}
)
target_link_libraries(radio_util_sdr
    hackrf
    rtlsdr
    LimeSuite
)

## radio_util_ui lib
add_library(radio_util_ui ${LIB_UI_SRC})
target_include_directories(radio_util_ui
    PUBLIC ${ALL_INCLUDE_DIRS}
)
target_link_libraries(radio_util_ui
    SDL2
    SDL2_ttf
    fftw3f
)

## demo app
add_executable(demo
    src/demo.cpp
)
target_include_directories(demo
    PUBLIC ${ALL_INCLUDE_DIRS}
)
target_link_libraries(demo
    radio_util_sdr
    radio_util_ui
)