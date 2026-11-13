//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unit tests for Linux startup cf_protection helpers.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/properties/architectures.h"
#include "startup/linux/cf_protection.h"
#include "test/UnitTest/ExecuteFunction.h"
#include "test/UnitTest/Test.h"

#include <sys/syscall.h>

namespace {

[[gnu::noinline, maybe_unused]] int tracked_call_probe(int x) { return x + 1; }

} // namespace

TEST(LlvmLibcCfProtectionTest, CpuCompatibilityCheckIsConsistent) {
  bool compat_first = LIBC_NAMESPACE::is_cpu_cf_protection_return_compatible();
  bool compat_second = LIBC_NAMESPACE::is_cpu_cf_protection_return_compatible();
  ASSERT_EQ(compat_first, compat_second);

#if defined(LIBC_TARGET_ARCH_IS_X86_64)
  uint64_t shstk_status = 0;
  long status_ret = LIBC_NAMESPACE::syscall_impl<long>(
      SYS_arch_prctl, ARCH_SHSTK_STATUS, reinterpret_cast<long>(&shstk_status));
  // If SHSTK is currently active in the kernel, the CPU must support CET_SS.
  if (status_ret == 0 && (shstk_status & ARCH_SHSTK_SHSTK) != 0)
    EXPECT_TRUE(compat_first);
#else
  EXPECT_FALSE(compat_first);
  LIBC_NAMESPACE::ErrorOr<void> res =
      LIBC_NAMESPACE::enable_cf_protection_return();
  EXPECT_FALSE(res.has_value());
#endif
}

TEST(LlvmLibcCfProtectionTest, EnableCfProtectionReturnInSubprocess) {
  LIBC_NAMESPACE::testutils::ProcessStatus proc =
      LIBC_NAMESPACE::testutils::invoke_in_subprocess([]() {
        bool cpu_compat =
            LIBC_NAMESPACE::is_cpu_cf_protection_return_compatible();
        LIBC_NAMESPACE::ErrorOr<void> res =
            LIBC_NAMESPACE::enable_cf_protection_return();

        if (!cpu_compat) {
          long code = res.has_value() ? 1 : 0;
          LIBC_NAMESPACE::syscall_impl<long>(SYS_exit_group, code);
        }

#if defined(LIBC_TARGET_ARCH_IS_X86_64)
        uint64_t status = 0;
        long status_ret = LIBC_NAMESPACE::syscall_impl<long>(
            SYS_arch_prctl, ARCH_SHSTK_STATUS, reinterpret_cast<long>(&status));
        if (res.has_value()) {
          if (status_ret != 0 || (status & ARCH_SHSTK_SHSTK) == 0)
            LIBC_NAMESPACE::syscall_impl<long>(SYS_exit_group, 2);
          if (tracked_call_probe(41) != 42)
            LIBC_NAMESPACE::syscall_impl<long>(SYS_exit_group, 3);
        } else {
          int err = res.error();
          if (err != EINVAL && err != EOPNOTSUPP && err != ENOSYS &&
              err != EPERM)
            LIBC_NAMESPACE::syscall_impl<long>(SYS_exit_group, 4);
        }
#endif
        LIBC_NAMESPACE::syscall_impl<long>(SYS_exit_group, 0);
      });

  ASSERT_TRUE(proc.exited_normally());
  ASSERT_EQ(proc.get_exit_code(), 0);
}
