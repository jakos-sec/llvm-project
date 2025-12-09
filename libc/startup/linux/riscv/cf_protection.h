//===-- Header file of cf_protection --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIBC_STARTUP_LINUX_RISCV_CF_PROTECTION_H
#define LLVM_LIBC_STARTUP_LINUX_RISCV_CF_PROTECTION_H

#include "src/__support/macros/config.h"
#include "src/__support/macros/attributes.h" // LIBC_INLINE

namespace LIBC_NAMESPACE_DECL {

// We currently do not support cf_protection on riscv, but should hook up
// support for Zicfilp (forward-edge) and Zicfiss (backward-edge).
[[gnu::always_inline]] LIBC_INLINE bool is_cpu_cf_protection_return_compatible() {
  return false;
}

[[gnu::always_inline]] LIBC_INLINE bool enable_cf_protection_return() {
  return false;
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_STARTUP_LINUX_RISCV_CF_PROTECTION_H
