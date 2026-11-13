if(NOT (EXISTS ${LIBC_SOURCE_DIR}/startup/linux/${LIBC_TARGET_ARCHITECTURE}))
  message("Skipping startup integration tests for target architecture ${LIBC_TARGET_ARCHITECTURE}.")
  return()
endif()

add_libc_testsuite(libc-startup-tests PARENT libc-integration-tests)

add_integration_test(
  startup_args_test
  SUITE libc-startup-tests
  SRCS
    args_test.cpp
  ARGS
    1 2 3
  ENV
    FRANCE=Paris
    GERMANY=Berlin
)

add_integration_test(
  invocation_name_test
  SUITE libc-startup-tests
  SRCS
    invocation_name_test.cpp
  DEPENDS
    libc.src.__support.CPP.string_view
    libc.src.errno.program_invocation_name
    libc.src.errno.program_invocation_short_name
    libc.src.unistd.execve
)

add_integration_test(
  startup_no_envp_test
  SUITE libc-startup-tests
  SRCS
    main_without_envp.cpp
)

add_integration_test(
  startup_no_args_test
  SUITE libc-startup-tests
  SRCS
    main_without_args.cpp
)

add_integration_test(
  startup_tls_test
  SUITE libc-startup-tests
  SRCS
    tls_test.cpp
  DEPENDS
    libc.hdr.sys_mman_macros
    libc.include.sys_mman
    libc.src.sys.mman.mmap
)

add_integration_test(
  init_fini_array_test
  SUITE libc-startup-tests
  SRCS
    init_fini_array_test.cpp
)

add_integration_test(
  startup_irelative_test
  SUITE libc-startup-tests
  SRCS
    irelative_test.cpp
)

add_integration_test(
  startup_irelative_no_ifunc_test
  SUITE libc-startup-tests
  SRCS
    irelative_no_ifunc_test.cpp
)

add_integration_test(
  startup_r_debug_test
  SUITE libc-startup-tests
  SRCS
    r_debug_test.cpp
  DEPENDS
    libc.include.link
    libc.src.link._r_debug
)

set(no_shstk_test_compile_options "")
if(LIBC_CC_SUPPORTS_FCF_PROTECTION)
  list(APPEND no_shstk_test_compile_options -fcf-protection=none)
endif()

add_integration_test(
  startup_cf_protection_shstk_test
  SUITE libc-startup-tests
  SRCS
    cf_protection_shstk_test.cpp
  DEPENDS
    libc.hdr.elf_proxy
    libc.hdr.link_macros
    libc.hdr.signal_macros
    libc.hdr.stdint_proxy
    libc.hdr.sys_auxv_macros
    libc.hdr.sys_wait_macros
    libc.include.sys_syscall
    libc.src.__support.OSUtil.osutil
    libc.src.__support.error_or
    libc.src.__support.macros.properties.architectures
    libc.src.link._r_debug
    libc.src.sys.auxv.getauxval
    libc.src.sys.wait.waitpid
    libc.src.unistd.fork
    libc.startup.linux.cf_protection
    libc.startup.linux.gnu_property_section
)

add_integration_test(
  startup_cf_protection_no_shstk_test
  SUITE libc-startup-tests
  SRCS
    cf_protection_no_shstk_test.cpp
  COMPILE_OPTIONS
    ${no_shstk_test_compile_options}
  DEPENDS
    libc.hdr.elf_proxy
    libc.hdr.link_macros
    libc.hdr.stdint_proxy
    libc.hdr.sys_auxv_macros
    libc.include.sys_syscall
    libc.src.__support.OSUtil.osutil
    libc.src.__support.error_or
    libc.src.__support.macros.properties.architectures
    libc.src.link._r_debug
    libc.src.sys.auxv.getauxval
    libc.startup.linux.cf_protection
    libc.startup.linux.gnu_property_section
)
