add_library(displayLib STATIC
    src/fonts.c
    src/st7735.c
)

#add target libraries
target_link_libraries(displayLib pico_stdlib hardware_gpio hardware_spi)
target_include_directories(displayLib PUBLIC inc/)
