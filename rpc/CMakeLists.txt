file(GLOB th_src thallium/*.cpp)

add_library(as-rpc SHARED ${th_src})
target_include_directories(as-rpc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(as-rpc PUBLIC thallium) # PUBLIC because there is an include thallium in the rpc headers
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries(as-rpc PRIVATE thallium_check_types)
endif()
install(TARGETS as-rpc)
