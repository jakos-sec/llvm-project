//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test verifying SHSTK stays disabled for non-SHSTK binaries.
///
//===----------------------------------------------------------------------===//

#include "hdr/elf_proxy.h"
#include "hdr/link_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/sys_auxv_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/link/_r_debug.h"
#include "src/sys/auxv/getauxval.h"
#include "startup/linux/cf_protection.h"
#include "startup/linux/gnu_property_section.h"
#include "test/IntegrationTest/test.h"

#include <sys/syscall.h>

TEST_MAIN() {
  const auto *phdrs =
      reinterpret_cast<const ElfW(Phdr) *>(LIBC_NAMESPACE::getauxval(AT_PHDR));
  unsigned long phnum = LIBC_NAMESPACE::getauxval(AT_PHNUM);

  const ElfW(Phdr) *gnu_phdr = nullptr;
  for (unsigned long i = 0; phdrs && i < phnum; ++i) {
    if (phdrs[i].p_type == PT_GNU_PROPERTY) {
      gnu_phdr = &phdrs[i];
      break;
    }
  }

  if (gnu_phdr) {
    LIBC_NAMESPACE::GnuPropertySection gnu_property;
    if (gnu_property.parse(gnu_phdr, LIBC_NAMESPACE::_r_debug.r_ldbase))
      ASSERT_FALSE(gnu_property.is_shstk_supported());
  }

#if defined(LIBC_TARGET_ARCH_IS_X86_64)
  uint64_t shstk_status = 0;
  long status_ret = LIBC_NAMESPACE::syscall_impl<long>(
      SYS_arch_prctl, ARCH_SHSTK_STATUS, reinterpret_cast<long>(&shstk_status));
  if (status_ret == 0)
    ASSERT_EQ(shstk_status & ARCH_SHSTK_SHSTK, static_cast<uint64_t>(0));
#endif

  return 0;
}
