bool uc_arch_supported(uc_arch arch)
{
    switch (arch) {
#ifdef UNICORN_HAS_ARM
    case UC_ARCH_ARM:
        return true;
#endif
#ifdef UNICORN_HAS_ARM64
    case UC_ARCH_ARM64:
        return true;
#endif
#ifdef UNICORN_HAS_M68K
    case UC_ARCH_M68K:
        return true;
#endif
#ifdef UNICORN_HAS_MIPS
    case UC_ARCH_MIPS:
        return true;
#endif
#ifdef UNICORN_HAS_PPC
    case UC_ARCH_PPC:
        return true;
#endif
#ifdef UNICORN_HAS_SPARC
    case UC_ARCH_SPARC:
        return true;
#endif
#ifdef UNICORN_HAS_X86
    case UC_ARCH_X86:
        return true;
#endif
#ifdef UNICORN_HAS_RISCV
    case UC_ARCH_RISCV:
        return true;
#endif
    /* Invalid or disabled arch */
    default:
        return false;
    }
}