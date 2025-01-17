idf_component_register(
        SRCS
        "DAPlink-master.c"
        "daplink/Source/DAP.c"
        "daplink/Source/DAP_vendor.c"
        "daplink/Source/JTAG_DP.c"
        "daplink/Source/SW_DP.c"
        "daplink/Source/SWD_flash.c"
        "daplink/Source/SWD_host.c"
        "daplink/Source/SWD_opt.c"
        "daplink/Source/error.c"
        "daplink/algo/STM32_ALGO.c"
        "daplink/algo/STM32F0xx_OPT.c"
        "daplink/algo/STM32F10x_OPT.c"
        "daplink/algo/STM32F3xx_OPT.c"
        "daplink/algo/STM32F4xx_OPT.c"
        "daplink/algo/STM32F7xx_OPT.c"
        "daplink/algo/STM32H7xx.c"
        "usb/usb_descriptors.c"
        "usb/hid_task.c"
        "usb/cdc_task.c"
        "usb/msc_task.c"
        "usb/usb.c"
        "lcd/lcd.c"
        "gui/gui.c"
        "gui/welcome_ui.c"
        "wifi/wifi.c"
        "vfs/vfs.c"
                    INCLUDE_DIRS
        "."
        "daplink/Include"
        "lcd/include"
        "gui/include"
        "wifi/include"
        "usb/include"
        "vfs/include"
)
idf_component_get_property(tusb_lib espressif__tinyusb COMPONENT_LIB)
cmake_policy(SET CMP0079 NEW)
target_link_libraries(${tusb_lib} PRIVATE ${COMPONENT_LIB})

lvgl_port_create_c_image("gui/images/connect.png" "gui/images/" "ARGB8888" "NONE")
lvgl_port_create_c_image("gui/images/disconnect.png" "gui/images/" "ARGB8888" "NONE")

lvgl_port_create_c_image("gui/images/esp_logo.png" "gui/images/" "ARGB8888" "NONE")
lvgl_port_create_c_image("gui/images/esp_text.png" "gui/images/" "ARGB8888" "NONE")
lvgl_port_add_images(${COMPONENT_LIB} "gui/images/")

set_source_files_properties(
        PROPERTIES COMPILE_OPTIONS
        "-DLV_LVGL_H_INCLUDE_SIMPLE;-Wno-format;"
)
#spiffs_create_partition_image(fonts ./gui/fonts FLASH_IN_PROJECT)
