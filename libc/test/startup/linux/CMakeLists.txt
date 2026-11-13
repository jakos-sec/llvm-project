if(NOT TARGET libc.startup.linux.gnu_property_section)
  return()
endif()

add_libc_testsuite(libc-startup-unit-tests)

add_libc_test(
  gnu_property_section_test
  FULL_BUILD_ONLY
  SUITE
    libc-startup-unit-tests
  SRCS
    gnu_property_section_test.cpp
  DEPENDS
    libc.hdr.elf_proxy
    libc.hdr.link_macros
    libc.hdr.stdint_proxy
    libc.src.__support.macros.properties.architectures
    libc.startup.linux.gnu_property_section
)

if(TARGET libc.startup.linux.cf_protection)
  set(cf_protection_test_compile_options "")
  if(LIBC_CC_SUPPORTS_FCF_PROTECTION)
    list(APPEND cf_protection_test_compile_options -fcf-protection=full)
  endif()

  add_libc_test(
    cf_protection_test
    FULL_BUILD_ONLY
    SUITE
      libc-startup-unit-tests
    SRCS
      cf_protection_test.cpp
    COMPILE_OPTIONS
      ${cf_protection_test_compile_options}
    DEPENDS
      libc.hdr.errno_macros
      libc.hdr.stdint_proxy
      libc.include.sys_syscall
      libc.src.__support.OSUtil.osutil
      libc.src.__support.error_or
      libc.src.__support.macros.properties.architectures
      libc.startup.linux.cf_protection
  )
endif()
