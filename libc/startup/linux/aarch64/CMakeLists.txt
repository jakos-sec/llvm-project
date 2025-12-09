add_startup_object(
  tls
  SRC
    tls.cpp
  DEPENDS
    libc.config.app_h
    libc.include.sys_mman
    libc.include.sys_syscall
    libc.src.__support.OSUtil.osutil
    libc.src.string.memory_utils.inline_memcpy
  COMPILE_OPTIONS
    -fno-omit-frame-pointer
    -ffreestanding # To avoid compiler warnings about calling the main function.
)

add_startup_object(
  start
  SRC
    start.cpp
  DEPENDS
    libc.config.app_h
  COMPILE_OPTIONS
    -fno-omit-frame-pointer
    -ffreestanding # To avoid compiler warnings about calling the main function.
)

add_startup_object(
  irelative
  SRC
    irelative.cpp
  DEPENDS
    libc.hdr.link_macros
    libc.hdr.elf_macros
    libc.hdr.elf_proxy
    libc.hdr.stdint_proxy
    libc.src.__support.macros.config
  COMPILE_OPTIONS
    -fno-stack-protector
    -fno-omit-frame-pointer
    -ffreestanding
    -fno-builtin
)

add_startup_object(
  cf_protection
  HDRS
    cf_protection.h
  DEPENDS
    libc.src.__support.macros.config
  COMPILE_OPTIONS
    -fno-stack-protector
    -fno-omit-frame-pointer
    -ffreestanding
    -fno-builtin
)
