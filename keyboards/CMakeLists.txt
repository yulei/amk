
add_library(m20add STATIC "")

target_sources(m20add
    PRIVATE
        m20add/m20add.c)

target_link_libraries(m20add tmk)
target_include_directories(m20add
    PUBLIC
        m20add)
