file(GLOB tl_src thallium/*.cpp)
file(GLOB headers include/as*.hpp)

add_library(as-rpc SHARED ${tl_src})
target_sources(as-rpc
  PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
  FILES ${headers}
)
target_link_libraries(as-rpc PUBLIC thallium) # PUBLIC because there is an include thallium in the rpc headers
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries(as-rpc PRIVATE thallium_check_types)
endif()
install(TARGETS as-rpc FILE_SET HEADERS)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
