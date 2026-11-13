//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test verifying SHSTK startup enablement and ROP detection.
///
//===----------------------------------------------------------------------===//

#include "hdr/elf_proxy.h"
#include "hdr/link_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/sys_auxv_macros.h"
#include "hdr/sys_wait_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/properties/architectures.h"
#include "src/link/_r_debug.h"
#include "src/sys/auxv/getauxval.h"
#include "src/sys/wait/waitpid.h"
#include "src/unistd/fork.h"
#include "startup/linux/cf_protection.h"
#include "startup/linux/gnu_property_section.h"
#include "test/IntegrationTest/test.h"

#include <sys/syscall.h>

namespace {

bool constructor_ran = false;
#if defined(LIBC_TARGET_ARCH_IS_X86_64)
long constructor_status_ret = -1;
uint64_t constructor_shstk_status = 0;
#endif

[[gnu::constructor]] void record_init_array_shstk_status() {
  constructor_ran = true;
#if defined(LIBC_TARGET_ARCH_IS_X86_64)
  constructor_status_ret = LIBC_NAMESPACE::syscall_impl<long>(
      SYS_arch_prctl, ARCH_SHSTK_STATUS,
      reinterpret_cast<long>(&constructor_shstk_status));
#endif
}

#if defined(LIBC_TARGET_ARCH_IS_X86_64)
const ElfW(Phdr) * find_gnu_property_phdr() {
  const auto *phdrs =
      reinterpret_cast<const ElfW(Phdr) *>(LIBC_NAMESPACE::getauxval(AT_PHDR));
  unsigned long phnum = LIBC_NAMESPACE::getauxval(AT_PHNUM);
  if (!phdrs)
    return nullptr;
  for (unsigned long i = 0; i < phnum; ++i) {
    if (phdrs[i].p_type == PT_GNU_PROPERTY)
      return &phdrs[i];
  }
  return nullptr;
}

// Pushes a synthetic target onto the data stack only and executes `retq`.
// - If hardware SHSTK is active, `retq` compares `1f` against the shadow stack
//   return address and faults with #CP (SIGSEGV).
// - If SHSTK is inactive, `retq` jumps cleanly to `1:` with %rsp balanced.
[[gnu::noinline]] void trigger_shadow_stack_mismatch() {
  asm volatile("leaq 1f(%%rip), %%rax\n\t"
               "pushq %%rax\n\t"
               "retq\n"
               "1:\n\t"
               :
               :
               : "rax", "memory");
}
#endif

} // namespace

TEST_MAIN() {
  ASSERT_TRUE(constructor_ran);

#if defined(LIBC_TARGET_ARCH_IS_X86_64)
  uint64_t shstk_status = 0;
  long status_ret = LIBC_NAMESPACE::syscall_impl<long>(
      SYS_arch_prctl, ARCH_SHSTK_STATUS, reinterpret_cast<long>(&shstk_status));

  bool expect_shstk_active = false;
#if defined(LIBC_COPT_ENABLE_CF_PROTECTION_FULL)
  const ElfW(Phdr) *gnu_phdr = find_gnu_property_phdr();
  ASSERT_NE(gnu_phdr, static_cast<const ElfW(Phdr) *>(nullptr));

  LIBC_NAMESPACE::GnuPropertySection gnu_property;
  ASSERT_TRUE(gnu_property.parse(gnu_phdr, LIBC_NAMESPACE::_r_debug.r_ldbase));
  ASSERT_TRUE(gnu_property.is_shstk_supported());

  if (status_ret == 0 && (shstk_status & ARCH_SHSTK_SHSTK) != 0) {
    expect_shstk_active = true;
    ASSERT_TRUE(LIBC_NAMESPACE::is_cpu_cf_protection_return_compatible());
    ASSERT_EQ(constructor_status_ret, 0L);
    ASSERT_NE(constructor_shstk_status & ARCH_SHSTK_SHSTK,
              static_cast<uint64_t>(0));
  } else if (LIBC_NAMESPACE::is_cpu_cf_protection_return_compatible()) {
    // If the CPU supports CET_SS yet SHSTK is not active at main(), verify in a
    // child process that the kernel itself rejects ARCH_SHSTK_ENABLE (rather
    // than do_start failing to invoke enable_cf_protection_return()).
    pid_t probe_pid = LIBC_NAMESPACE::fork();
    if (probe_pid == 0) {
      LIBC_NAMESPACE::ErrorOr<void> res =
          LIBC_NAMESPACE::enable_cf_protection_return();
      long code = res.has_value() ? 1 : 0;
      LIBC_NAMESPACE::syscall_impl<long>(SYS_exit_group, code);
    }
    ASSERT_TRUE(probe_pid > 0);
    int probe_status = 0;
    ASSERT_EQ(LIBC_NAMESPACE::waitpid(probe_pid, &probe_status, 0), probe_pid);
    ASSERT_TRUE(WIFEXITED(probe_status));
    ASSERT_EQ(WEXITSTATUS(probe_status), 0);
  }
#else
  if (status_ret == 0)
    ASSERT_EQ(shstk_status & ARCH_SHSTK_SHSTK, static_cast<uint64_t>(0));
#endif

  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid == 0) {
    trigger_shadow_stack_mismatch();
    LIBC_NAMESPACE::syscall_impl<long>(SYS_exit_group, 0);
  }
  ASSERT_TRUE(pid > 0);
  int wstatus = 0;
  ASSERT_EQ(LIBC_NAMESPACE::waitpid(pid, &wstatus, 0), pid);
  if (expect_shstk_active) {
    ASSERT_TRUE(WIFSIGNALED(wstatus));
    ASSERT_EQ(WTERMSIG(wstatus), SIGSEGV);
  } else {
    ASSERT_TRUE(WIFEXITED(wstatus));
    ASSERT_EQ(WEXITSTATUS(wstatus), 0);
  }
#endif
  return 0;
}
