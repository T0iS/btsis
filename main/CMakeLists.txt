set(COMPONENT_SRCS "control.c" "main.c"
                    "server.c"
                    "connect.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()

spiffs_create_partition_image(spiffs ../html FLASH_IN_PROJECT)