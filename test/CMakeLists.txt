add_executable(
    test_grayscale
    test_grayscale.c
)

add_executable(
    test_binarization
    test_binarization.c
)

add_executable(
    test_quantization
    test_quantization.c
)

add_executable(
    test_avgpool
    test_avgpool.c
)

target_link_libraries(
    test_grayscale
    imgproc
)

target_link_libraries(
    test_binarization
    imgproc
)

target_link_libraries(
    test_quantization
    imgproc
)

target_link_libraries(
    test_avgpool
    imgproc
)
