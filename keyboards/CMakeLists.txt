
add_library(kbd STATIC "")

target_link_libraries(kbd tmk)

if (MCU_TYPE MATCHES "NRF52840")
    target_sources(m20add
        PRIVATE
            m20addble/m20add.c)
    target_include_directories(m20add
        PUBLIC
            m20addble)

elseif (MCU_TYPE MATCHES "NRF52832")
    target_sources(kbd
        PRIVATE
            hhkbble/hhkbble.c)
    target_include_directories(kbd
        PUBLIC
            hhkbble)

elseif(MCU_TYPE MATCHES "STM32F411")
    target_sources(m20add
        PRIVATE
            m20add/m20add.c)
    target_include_directories(m20add
        PUBLIC
            m20add)
else()
    message(ERROR "unknown platform on main application building")
endif()