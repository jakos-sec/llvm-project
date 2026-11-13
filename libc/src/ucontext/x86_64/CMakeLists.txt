# TODO: Remove -fcf-protection=none once x86_64 getcontext/setcontext emit
# endbr64 and save/restore the hardware shadow stack pointer (SSP).
set(ucontext_cf_protection_flags "")
if(LIBC_CONF_ENABLE_CF_PROTECTION_FULL AND LIBC_CC_SUPPORTS_FCF_PROTECTION)
  list(APPEND ucontext_cf_protection_flags -fcf-protection=none)
endif()

add_entrypoint_object(
  getcontext
  SRCS
    getcontext.cpp
  HDRS
    ../getcontext.h
  COMPILE_OPTIONS
    ${ucontext_cf_protection_flags}
  DEPENDS
    libc.include.llvm-libc-types.ucontext_t
    libc.include.sys_syscall
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.hdr.types.size_t
)

add_entrypoint_object(
  setcontext
  SRCS
    setcontext.cpp
  HDRS
    ../setcontext.h
  COMPILE_OPTIONS
    ${ucontext_cf_protection_flags}
  DEPENDS
    libc.include.llvm-libc-types.ucontext_t
    libc.include.sys_syscall
    libc.src.__support.common
    libc.src.__support.macros.config
    libc.hdr.types.size_t
)
