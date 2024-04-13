static void find_context_reg_rw_function(uc_arch arch, uc_mode mode,
                                         context_reg_rw_t *rw)
{
    // We believe that the arch/mode pair is correct.
    switch (arch) {
    default:
        rw->context_reg_read = NULL;
        rw->context_reg_write = NULL;
        break;
#ifdef UNICORN_HAS_M68K
    case UC_ARCH_M68K:
        rw->context_reg_read = m68k_context_reg_read;
        rw->context_reg_write = m68k_context_reg_write;
        break;
#endif
#ifdef UNICORN_HAS_X86
    case UC_ARCH_X86:
        rw->context_reg_read = x86_context_reg_read;
        rw->context_reg_write = x86_context_reg_write;
        break;
#endif
#ifdef UNICORN_HAS_ARM
    case UC_ARCH_ARM:
        if (mode & UC_MODE_BIG_ENDIAN) {
            rw->context_reg_read = armeb_context_reg_read;
            rw->context_reg_write = armeb_context_reg_write;
        } else {
            rw->context_reg_read = arm_context_reg_read;
            rw->context_reg_write = arm_context_reg_write;
        }
#endif
#ifdef UNICORN_HAS_ARM64
    case UC_ARCH_ARM64:
        if (mode & UC_MODE_BIG_ENDIAN) {
            rw->context_reg_read = arm64eb_context_reg_read;
            rw->context_reg_write = arm64eb_context_reg_write;
        } else {
            rw->context_reg_read = arm64_context_reg_read;
            rw->context_reg_write = arm64_context_reg_write;
        }
        break;
#endif

#if defined(UNICORN_HAS_MIPS) || defined(UNICORN_HAS_MIPSEL) ||                \
    defined(UNICORN_HAS_MIPS64) || defined(UNICORN_HAS_MIPS64EL)
    case UC_ARCH_MIPS:
        if (mode & UC_MODE_BIG_ENDIAN) {
#ifdef UNICORN_HAS_MIPS
            if (mode & UC_MODE_MIPS32) {
                rw->context_reg_read = mips_context_reg_read;
                rw->context_reg_write = mips_context_reg_write;
            }
#endif
#ifdef UNICORN_HAS_MIPS64
            if (mode & UC_MODE_MIPS64) {
                rw->context_reg_read = mips64_context_reg_read;
                rw->context_reg_write = mips64_context_reg_write;
            }
#endif
        } else { // little endian
#ifdef UNICORN_HAS_MIPSEL
            if (mode & UC_MODE_MIPS32) {
                rw->context_reg_read = mipsel_context_reg_read;
                rw->context_reg_write = mipsel_context_reg_write;
            }
#endif
#ifdef UNICORN_HAS_MIPS64EL
            if (mode & UC_MODE_MIPS64) {
                rw->context_reg_read = mips64el_context_reg_read;
                rw->context_reg_write = mips64el_context_reg_write;
            }
#endif
        }
        break;
#endif

#ifdef UNICORN_HAS_SPARC
    case UC_ARCH_SPARC:
        if (mode & UC_MODE_SPARC64) {
            rw->context_reg_read = sparc64_context_reg_read;
            rw->context_reg_write = sparc64_context_reg_write;
        } else {
            rw->context_reg_read = sparc_context_reg_read;
            rw->context_reg_write = sparc_context_reg_write;
        }
        break;
#endif
#ifdef UNICORN_HAS_PPC
    case UC_ARCH_PPC:
        if (mode & UC_MODE_PPC64) {
            rw->context_reg_read = ppc64_context_reg_read;
            rw->context_reg_write = ppc64_context_reg_write;
        } else {
            rw->context_reg_read = ppc_context_reg_read;
            rw->context_reg_write = ppc_context_reg_write;
        }
        break;
#endif
#ifdef UNICORN_HAS_RISCV
    case UC_ARCH_RISCV:
        if (mode & UC_MODE_RISCV32) {
            rw->context_reg_read = riscv32_context_reg_read;
            rw->context_reg_write = riscv32_context_reg_write;
        } else if (mode & UC_MODE_RISCV64) {
            rw->context_reg_read = riscv64_context_reg_read;
            rw->context_reg_write = riscv64_context_reg_write;
        }
        break;
#endif
    }

    return;
}