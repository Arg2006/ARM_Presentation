# CMake required for pico-sdk.

cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(pico_display_demo)

pico_sdk_init()

add_executable(pico_display_demo
    src/main.c
    src/display/st7789.c
    src/display/fonts.c
    src/display/xpt2046.c
    src/network/udp_handler.c
    src/network/wifi_handler.c
)

# Add compile definitions if provided
if(DEFINED WIFI_SSID)
    target_compile_definitions(pico_display_demo PRIVATE WIFI_SSID="${WIFI_SSID}")
endif()

if(DEFINED WIFI_PASSWORD)
    target_compile_definitions(pico_display_demo PRIVATE WIFI_PASSWORD="${WIFI_PASSWORD}")
endif()

target_include_directories(pico_display_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_arch/include
    ${PICO_SDK_PATH}/lib/cyw43-driver/src
)

target_link_libraries(pico_display_demo
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_spi
    pico_lwip
)


pico_add_extra_outputs(pico_display_demo)

pico_enable_stdio_usb(pico_display_demo 1)
pico_enable_stdio_uart(pico_display_demo 0)

add_compile_definitions(CYW43_DEBUG=printf)
