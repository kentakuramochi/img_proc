add_executable(
    read_write_pnm
    read_write_pnm.c
)

target_link_libraries(
    read_write_pnm
    imgproc
)
