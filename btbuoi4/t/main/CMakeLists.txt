idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS ""
    REQUIRES 
        esp_wifi
        esp_event
        esp_netif
        mqtt
        nvs_flash
        driver
        json
        esp_timer
        esp_system
)
