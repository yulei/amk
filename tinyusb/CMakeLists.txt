
set(TUSB_PATH "tinyusb/src")

add_library(tusb STATIC "")

target_include_directories(tusb
      PRIVATE
        ${CMAKE_CURRENT_LIST_DIR} 
      PUBLIC
        ${TUSB_PATH}
      )


target_sources(tusb
      PRIVATE
        ${TUSB_PATH}/tusb.c
        ${TUSB_PATH}/common/tusb_fifo.c
        ${TUSB_PATH}/device/usbd.c
        ${TUSB_PATH}/device/usbd_control.c
        ${TUSB_PATH}/class/cdc/cdc_device.c
        ${TUSB_PATH}/class/dfu/dfu_rt_device.c
        ${TUSB_PATH}/class/hid/hid_device.c
        ${TUSB_PATH}/class/midi/midi_device.c
        ${TUSB_PATH}/class/msc/msc_device.c
        ${TUSB_PATH}/class/net/net_device.c
        ${TUSB_PATH}/class/usbtmc/usbtmc_device.c
        ${TUSB_PATH}/class/vendor/vendor_device.c
      )

if (MCU_TYPE STREQUAL "NRF52840")
  target_sources(tusb
        PRIVATE
        ${TUSB_PATH}/portable/nordic/nrf5x/dcd_nrf5x.c
  )
  target_link_libraries(tusb PUBLIC nrf5_sdk)
elseif(MCU_TYPE STREQUAL "STM32F411")
  target_sources(tusb
        PRIVATE 
        ${TUSB_PATH}/portable/st/synopsys/dcd_synopsys.c
  )
  target_link_libraries(tusb PUBLIC stm32_sdk)
else()
  message(ERROR "unknown platform on tinyusb building")
endif()

