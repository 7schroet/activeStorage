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
endmacro()
