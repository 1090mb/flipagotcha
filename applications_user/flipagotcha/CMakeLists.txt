# Build definition for the flipagotcha app
add_application(flipagotcha src/main.c)

target_sources(flipagotcha PRIVATE
    src/ui/view.c
    src/ui/draw.c
    src/hw/uart.c
)

target_link_libraries(flipagotcha PRIVATE
    furi
    gui
    storage
    uart
)

target_include_directories(flipagotcha PRIVATE
    src
    src/ui
    src/hw
    src/common
)