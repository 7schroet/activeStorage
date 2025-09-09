add_library(as-rpc SHARED)

set(public_headers
    as_rpc.hpp
    as_rpc_engine.hpp
    as_rpc_kernels.hpp
    as_rpc_macros.hpp
    as_rpc_protocols.hpp
    as_rpc_types.hpp
    as_rpc_utils.hpp
)
list(TRANSFORM public_headers PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/include/" OUTPUT_VARIABLE public_headers)

target_sources(as-rpc
  PRIVATE
    thallium/tl_engine.cpp
    thallium/tl_kernel_impl.cpp
    thallium/tl_kernels.cpp
    thallium/tl_protocols.cpp
    thallium/tl_utils.cpp
    include/tl_kernel_impl.hpp

  PUBLIC
  FILE_SET HEADERS
  BASE_DIRS include
  FILES ${public_headers}
)

target_link_libraries(as-rpc PUBLIC thallium) # PUBLIC because there is an include thallium in the rpc headers
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries(as-rpc PRIVATE thallium_check_types)
endif()
install(TARGETS as-rpc FILE_SET HEADERS)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
