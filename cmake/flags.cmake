macro(populate_flags flag-interface)
  include(CheckCCompilerFlag)
  include(CheckCXXCompilerFlag)

  set(
    flag_names
    Wall
    Wextra
    Wpedantic
    Wshadow
    Wnrvo
  )

  foreach(flag_name ${flag_names})
    set(flag "-${flag_name}")
    check_cxx_compiler_flag(
      ${flag}
      ${flag_name}_CXX_WORKS
    )
    check_c_compiler_flag(
      ${flag}
      ${flag_name}_C_WORKS
    )
    if(${flag_name}_CXX_WORKS)
      target_compile_options(
        ${flag-interface}
        INTERFACE
          "$<$<COMPILE_LANGUAGE:CXX>:${flag}>"
      )
    endif()
    if(${flag_name}_C_WORKS)
      target_compile_options(
        ${flag-interface}
        INTERFACE
          "$<$<COMPILE_LANGUAGE:C>:${flag}>"
      )
    endif()
  endforeach()

  if(ENABLE_SANITIZER)
    set(sanitizer_flag -fsanitize=address,leak,undefined)
    if(DEFINED CMAKE_REQUIRED_LINK_OPTIONS AND CMAKE_REQUIRED_LINK_OPTIONS)
      set(prev_link_options ${CMAKE_REQUIRED_LINK_OPTIONS})
    else()
      set(prev_link_options)
    endif()

    set(CMAKE_REQUIRED_LINK_OPTIONS ${sanitizer_flag})
    check_cxx_compiler_flag(
      ${sanitizer_flag}
      SANITIZER_CXX_WORKS
    )
    check_c_compiler_flag(
      ${sanitizer_flag}
      SANITIZER_C_WORKS
    )
    if(SANITIZER_CXX_WORKS)
      target_compile_options(
        ${flag-interface}
        INTERFACE
          "$<$<COMPILE_LANGUAGE:CXX>:${sanitizer_flag}>"
      )
      target_link_options(
        ${flag-interface}
        INTERFACE
          "$<$<COMPILE_LANGUAGE:CXX>:${sanitizer_flag}>"
      )
    endif()
    if(SANITIZER_C_WORKS)
      target_compile_options(
        ${flag-interface}
        INTERFACE
          "$<$<COMPILE_LANGUAGE:C>:${sanitizer_flag}>"
      )
      target_link_options(
        ${flag-interface}
        INTERFACE
          "$<$<COMPILE_LANGUAGE:C>:${sanitizer_flag}>"
      )
    endif()

    set(CMAKE_REQUIRED_LINK_OPTIONS ${prev_link_options})
  endif()
endmacro()
