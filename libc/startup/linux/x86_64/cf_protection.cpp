//===-- Implementation file of cf_protection ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "startup/linux/x86_64/cf_protection.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

static inline void cpuid(uint32_t eax_in, uint32_t ecx_in, uint32_t *eax,
                         uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  asm volatile("cpuid"
               : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
               : "a"(eax_in), "c"(ecx_in));
}

LIBC_INLINE bool is_cpu_cf_protection_return_compatible() {
  uint32_t eax, ebx, ecx, edx;
  cpuid(0, 0, &eax, &ebx, &ecx, &edx);
  if (eax < 7)
    return false;

  // TODO: add more comments
  cpuid(7, 0, &eax, &ebx, &ecx, &edx);
  if (!(ecx & (1 << 7))) // CET_SS is bit 7 of ECX
    return false;

  return true;
}

} // namespace LIBC_NAMESPACE_DECL
