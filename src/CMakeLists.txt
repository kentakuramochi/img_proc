add_library(
    imgproc
    STATIC
    img.c
    imgproc.c
    pnm.c
)

target_include_directories(
    imgproc
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
