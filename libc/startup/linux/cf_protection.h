//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Architecture-dispatching header for Linux startup control-flow protection.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_STARTUP_LINUX_CF_PROTECTION_H
#define LLVM_LIBC_STARTUP_LINUX_CF_PROTECTION_H

#include "src/__support/macros/properties/architectures.h"

#if defined(LIBC_TARGET_ARCH_IS_X86_64)
#include "startup/linux/x86_64/cf_protection.h"
#elif defined(LIBC_TARGET_ARCH_IS_AARCH64)
#include "startup/linux/aarch64/cf_protection.h"
#elif defined(LIBC_TARGET_ARCH_IS_ANY_RISCV)
#include "startup/linux/riscv/cf_protection.h"
#else
#error "Unsupported architecture for Linux startup cf_protection"
#endif

#endif // LLVM_LIBC_STARTUP_LINUX_CF_PROTECTION_H
