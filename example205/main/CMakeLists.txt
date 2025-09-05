idf_component_register(SRCS "blink_main.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES spi_flash driver led_strip )