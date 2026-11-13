//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// AArch64 stub definitions for Linux startup control-flow protection.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_STARTUP_LINUX_AARCH64_CF_PROTECTION_H
#define LLVM_LIBC_STARTUP_LINUX_AARCH64_CF_PROTECTION_H

#include "hdr/errno_macros.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

/// Returns false on AArch64 until backward-edge return protection (such as
/// Guarded Control Stack / PAC via `-mbranch-protection`) is wired into
/// startup.
LIBC_INLINE bool is_cpu_cf_protection_return_compatible() { return false; }

/// Stub for AArch64 startup backward-edge control-flow protection enablement.
LIBC_ALWAYS_INLINE ErrorOr<void> enable_cf_protection_return() {
  return Error(ENOTSUP);
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_STARTUP_LINUX_AARCH64_CF_PROTECTION_H
