//===-- Implementation header for cf_protection ---------------- *- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_STARTUP_LINUX_CF_PROTECTION_H
#define LLVM_LIBC_STARTUP_LINUX_CF_PROTECTION_H

#if defined(LIBC_TARGET_ARCH_IS_X86)
#include "startup/linux/x86_64/cf_protection.h"
#elif defined(LIBC_TARGET_ARCH_IS_AARCH64)
#include "startup/linux/aarch64/cf_protection.h"
#elif defined(LIBC_TARGET_ARCH_IS_ANY_RISCV)
#include "startup/linux/riscv/cf_protection.h"
#else
#error "unknown arch"
#endif

#endif // LLVM_LIBC_STARTUP_LINUX_CF_PROTECTION_H
