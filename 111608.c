uc_err uc_open(uc_arch arch, uc_mode mode, uc_engine **result)
{
    struct uc_struct *uc;

    if (arch < UC_ARCH_MAX) {
        uc = calloc(1, sizeof(*uc));
        if (!uc) {
            // memory insufficient
            return UC_ERR_NOMEM;
        }

        /* qemu/exec.c: phys_map_node_reserve() */
        uc->alloc_hint = 16;
        uc->errnum = UC_ERR_OK;
        uc->arch = arch;
        uc->mode = mode;

        // uc->ram_list = { .blocks = QLIST_HEAD_INITIALIZER(ram_list.blocks) };
        QLIST_INIT(&uc->ram_list.blocks);

        QTAILQ_INIT(&uc->memory_listeners);

        QTAILQ_INIT(&uc->address_spaces);

        switch (arch) {
        default:
            break;
#ifdef UNICORN_HAS_M68K
        case UC_ARCH_M68K:
            if ((mode & ~UC_MODE_M68K_MASK) || !(mode & UC_MODE_BIG_ENDIAN)) {
                free(uc);
                return UC_ERR_MODE;
            }
            uc->init_arch = m68k_uc_init;
            break;
#endif
#ifdef UNICORN_HAS_X86
        case UC_ARCH_X86:
            if ((mode & ~UC_MODE_X86_MASK) || (mode & UC_MODE_BIG_ENDIAN) ||
                !(mode & (UC_MODE_16 | UC_MODE_32 | UC_MODE_64))) {
                free(uc);
                return UC_ERR_MODE;
            }
            uc->init_arch = x86_uc_init;
            break;
#endif
#ifdef UNICORN_HAS_ARM
        case UC_ARCH_ARM:
            if ((mode & ~UC_MODE_ARM_MASK)) {
                free(uc);
                return UC_ERR_MODE;
            }
            if (mode & UC_MODE_BIG_ENDIAN) {
                uc->init_arch = armeb_uc_init;
            } else {
                uc->init_arch = arm_uc_init;
            }

            if (mode & UC_MODE_THUMB) {
                uc->thumb = 1;
            }
            break;
#endif
#ifdef UNICORN_HAS_ARM64
        case UC_ARCH_ARM64:
            if (mode & ~UC_MODE_ARM_MASK) {
                free(uc);
                return UC_ERR_MODE;
            }
            if (mode & UC_MODE_BIG_ENDIAN) {
                uc->init_arch = arm64eb_uc_init;
            } else {
                uc->init_arch = arm64_uc_init;
            }
            break;
#endif

#if defined(UNICORN_HAS_MIPS) || defined(UNICORN_HAS_MIPSEL) ||                \
    defined(UNICORN_HAS_MIPS64) || defined(UNICORN_HAS_MIPS64EL)
        case UC_ARCH_MIPS:
            if ((mode & ~UC_MODE_MIPS_MASK) ||
                !(mode & (UC_MODE_MIPS32 | UC_MODE_MIPS64))) {
                free(uc);
                return UC_ERR_MODE;
            }
            if (mode & UC_MODE_BIG_ENDIAN) {
#ifdef UNICORN_HAS_MIPS
                if (mode & UC_MODE_MIPS32) {
                    uc->init_arch = mips_uc_init;
                }
#endif
#ifdef UNICORN_HAS_MIPS64
                if (mode & UC_MODE_MIPS64) {
                    uc->init_arch = mips64_uc_init;
                }
#endif
            } else { // little endian
#ifdef UNICORN_HAS_MIPSEL
                if (mode & UC_MODE_MIPS32) {
                    uc->init_arch = mipsel_uc_init;
                }
#endif
#ifdef UNICORN_HAS_MIPS64EL
                if (mode & UC_MODE_MIPS64) {
                    uc->init_arch = mips64el_uc_init;
                }
#endif
            }
            break;
#endif

#ifdef UNICORN_HAS_SPARC
        case UC_ARCH_SPARC:
            if ((mode & ~UC_MODE_SPARC_MASK) || !(mode & UC_MODE_BIG_ENDIAN) ||
                !(mode & (UC_MODE_SPARC32 | UC_MODE_SPARC64))) {
                free(uc);
                return UC_ERR_MODE;
            }
            if (mode & UC_MODE_SPARC64) {
                uc->init_arch = sparc64_uc_init;
            } else {
                uc->init_arch = sparc_uc_init;
            }
            break;
#endif
#ifdef UNICORN_HAS_PPC
        case UC_ARCH_PPC:
            if ((mode & ~UC_MODE_PPC_MASK) || !(mode & UC_MODE_BIG_ENDIAN) ||
                !(mode & (UC_MODE_PPC32 | UC_MODE_PPC64))) {
                free(uc);
                return UC_ERR_MODE;
            }
            if (mode & UC_MODE_PPC64) {
                uc->init_arch = ppc64_uc_init;
            } else {
                uc->init_arch = ppc_uc_init;
            }
            break;
#endif
#ifdef UNICORN_HAS_RISCV
        case UC_ARCH_RISCV:
            if ((mode & ~UC_MODE_RISCV_MASK) ||
                !(mode & (UC_MODE_RISCV32 | UC_MODE_RISCV64))) {
                free(uc);
                return UC_ERR_MODE;
            }
            if (mode & UC_MODE_RISCV32) {
                uc->init_arch = riscv32_uc_init;
            } else if (mode & UC_MODE_RISCV64) {
                uc->init_arch = riscv64_uc_init;
            } else {
                free(uc);
                return UC_ERR_MODE;
            }
            break;
#endif
        }

        if (uc->init_arch == NULL) {
            return UC_ERR_ARCH;
        }

        uc->init_done = false;
        uc->cpu_model = INT_MAX; // INT_MAX means the default cpu model.

        *result = uc;

        return UC_ERR_OK;
    } else {
        return UC_ERR_ARCH;
    }
}