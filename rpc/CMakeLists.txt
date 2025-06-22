file(GLOB hg_src mercury/*.c)

add_library(as-rpc SHARED ${hg_src})
target_include_directories(as-rpc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(as-rpc PRIVATE mercury)
