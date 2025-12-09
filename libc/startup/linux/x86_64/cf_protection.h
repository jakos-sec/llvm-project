//===-- Header file of cf_protection --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIBC_STARTUP_LINUX_X86_64_CF_PROTECTION_H
#define LLVM_LIBC_STARTUP_LINUX_X86_64_CF_PROTECTION_H

#include "src/__support/macros/config.h"
#include "src/__support/macros/attributes.h" // LIBC_INLINE
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.

#include <stdint.h>
#include <asm/prctl.h>

namespace LIBC_NAMESPACE_DECL {

// Should be defined by kernel header files but might not be present yet.
#ifndef ARCH_SHSTK_ENABLE
# define ARCH_SHSTK_ENABLE    0x5001
# define ARCH_SHSTK_DISABLE   0x5002
# define ARCH_SHSTK_LOCK      0x5003
# define ARCH_SHSTK_UNLOCK    0x5004
# define ARCH_SHSTK_STATUS    0x5005

/* ARCH_SHSTK_ features bits */
# define ARCH_SHSTK_SHSTK     0x1
# define ARCH_SHSTK_WRSS      0x2
#endif

LIBC_INLINE bool is_cpu_cf_protection_return_compatible();

[[gnu::always_inline]] LIBC_INLINE bool enable_cf_protection_return() {
  // TODO add comment
  long long int kernel_feature = ARCH_SHSTK_SHSTK;
  auto ret = syscall_impl(SYS_arch_prctl, ARCH_SHSTK_ENABLE, kernel_feature);
  return ret > -1;
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_STARTUP_LINUX_X86_64_CF_PROTECTION_H
