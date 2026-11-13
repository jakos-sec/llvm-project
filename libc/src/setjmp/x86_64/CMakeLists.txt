# TODO: Remove -fcf-protection=none once x86_64 setjmp/longjmp/sigsetjmp emit
# endbr64 and unwind the hardware shadow stack pointer (SSP). Keeping
# -fcf-protection=none ensures the linker clears GNU_PROPERTY_X86_FEATURE_1_SHSTK
# for binaries that link non-CET-aware setjmp/longjmp implementations.
set(setjmp_cf_protection_flags "")
if(LIBC_CONF_ENABLE_CF_PROTECTION_FULL AND LIBC_CC_SUPPORTS_FCF_PROTECTION)
  list(APPEND setjmp_cf_protection_flags -fcf-protection=none)
endif()

add_entrypoint_object(
  setjmp
  SRCS
    setjmp.cpp
  HDRS
    ../setjmp_impl.h
  COMPILE_OPTIONS
    ${setjmp_cf_protection_flags}
  DEPENDS
    libc.hdr.offsetof_macros
    libc.hdr.types.jmp_buf
)
if(TARGET libc.src.setjmp.sigsetjmp_epilogue)
  add_entrypoint_object(
    sigsetjmp
    SRCS
      sigsetjmp.cpp
    HDRS
      ../sigsetjmp.h
    COMPILE_OPTIONS
      ${setjmp_cf_protection_flags}
    DEPENDS
      libc.hdr.types.sigjmp_buf
      libc.hdr.types.sigset_t
      libc.hdr.offsetof_macros
      libc.src.setjmp.sigsetjmp_epilogue
      libc.src.setjmp.setjmp
  )
endif()

add_entrypoint_object(
  longjmp
  SRCS
    longjmp.cpp
  HDRS
    ../longjmp.h
  COMPILE_OPTIONS
    ${setjmp_cf_protection_flags}
  DEPENDS
    libc.hdr.types.jmp_buf
)
