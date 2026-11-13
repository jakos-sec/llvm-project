//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// x86_64 Linux startup support for CET Shadow Stack (SHSTK) protection.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_STARTUP_LINUX_X86_64_CF_PROTECTION_H
#define LLVM_LIBC_STARTUP_LINUX_X86_64_CF_PROTECTION_H

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

#include <asm/prctl.h>
#include <cpuid.h>
#include <sys/syscall.h>

// Fallback definitions from Linux UAPI <asm/prctl.h> (added in Linux 6.6) for
// build environments with older kernel headers.
#ifndef ARCH_SHSTK_ENABLE
#define ARCH_SHSTK_ENABLE 0x5001
#define ARCH_SHSTK_DISABLE 0x5002
#define ARCH_SHSTK_LOCK 0x5003
#define ARCH_SHSTK_UNLOCK 0x5004
#define ARCH_SHSTK_STATUS 0x5005

// ARCH_SHSTK_* feature bits.
#define ARCH_SHSTK_SHSTK 0x1
#define ARCH_SHSTK_WRSS 0x2
#endif

namespace LIBC_NAMESPACE_DECL {

// Structured Extended Feature Flags Enumeration Leaf (EAX = 7, ECX = 0).
LIBC_INLINE_VAR constexpr unsigned int CPUID_EXTENDED_FEATURES_LEAF = 7;
LIBC_INLINE_VAR constexpr unsigned int CPUID_EXTENDED_FEATURES_SUBLEAF = 0;

// In CPUID.(EAX=07H, ECX=0H):ECX, bit 7 (CET_SS) indicates processor support
// for Control-flow Enforcement Technology Shadow Stacks.
LIBC_INLINE_VAR constexpr unsigned int CPUID_ECX_CET_SS = 1U << 7;

/// Returns true if the current x86_64 CPU supports CET Shadow Stack (CET_SS).
LIBC_INLINE bool is_cpu_cf_protection_return_compatible() {
  unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
  if (__get_cpuid_count(CPUID_EXTENDED_FEATURES_LEAF,
                        CPUID_EXTENDED_FEATURES_SUBLEAF, &eax, &ebx, &ecx,
                        &edx) == 0)
    return false;
  return (ecx & CPUID_ECX_CET_SS) != 0;
}

/// Enables CET Shadow Stack for the calling thread via `arch_prctl`.
///
/// Must be `always_inline` and called only from a `[[noreturn]]` startup frame
/// (such as `do_start`): if called out-of-line while SHSTK is still disabled,
/// `CALL` pushes the return address only onto the data stack, and `RET` after
/// `ARCH_SHSTK_ENABLE` activates the fresh hardware shadow stack would trigger
/// a Control Protection fault (`#CP` / `SIGSEGV`).
LIBC_ALWAYS_INLINE ErrorOr<void> enable_cf_protection_return() {
  constexpr unsigned long KERNEL_FEATURE = ARCH_SHSTK_SHSTK;
  long ret =
      syscall_impl<long>(SYS_arch_prctl, ARCH_SHSTK_ENABLE, KERNEL_FEATURE);
  if (ret < 0)
    return Error(static_cast<int>(-ret));
  return {};
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_STARTUP_LINUX_X86_64_CF_PROTECTION_H
