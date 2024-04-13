uc_err uc_emu_start(uc_engine *uc, uint64_t begin, uint64_t until,
                    uint64_t timeout, size_t count)
{
    // reset the counter
    uc->emu_counter = 0;
    uc->invalid_error = UC_ERR_OK;
    uc->emulation_done = false;
    uc->size_recur_mem = 0;
    uc->timed_out = false;
    uc->first_tb = true;

    UC_INIT(uc);

    // Advance the nested levels. We must decrease the level count by one when
    // we return from uc_emu_start.
    if (uc->nested_level >= UC_MAX_NESTED_LEVEL) {
        // We can't support so many nested levels.
        return UC_ERR_RESOURCE;
    }
    uc->nested_level++;

    switch (uc->arch) {
    default:
        break;
#ifdef UNICORN_HAS_M68K
    case UC_ARCH_M68K:
        uc_reg_write(uc, UC_M68K_REG_PC, &begin);
        break;
#endif
#ifdef UNICORN_HAS_X86
    case UC_ARCH_X86:
        switch (uc->mode) {
        default:
            break;
        case UC_MODE_16: {
            uint64_t ip;
            uint16_t cs;

            uc_reg_read(uc, UC_X86_REG_CS, &cs);
            // compensate for later adding up IP & CS
            ip = begin - cs * 16;
            uc_reg_write(uc, UC_X86_REG_IP, &ip);
            break;
        }
        case UC_MODE_32:
            uc_reg_write(uc, UC_X86_REG_EIP, &begin);
            break;
        case UC_MODE_64:
            uc_reg_write(uc, UC_X86_REG_RIP, &begin);
            break;
        }
        break;
#endif
#ifdef UNICORN_HAS_ARM
    case UC_ARCH_ARM:
        uc_reg_write(uc, UC_ARM_REG_R15, &begin);
        break;
#endif
#ifdef UNICORN_HAS_ARM64
    case UC_ARCH_ARM64:
        uc_reg_write(uc, UC_ARM64_REG_PC, &begin);
        break;
#endif
#ifdef UNICORN_HAS_MIPS
    case UC_ARCH_MIPS:
        // TODO: MIPS32/MIPS64/BIGENDIAN etc
        uc_reg_write(uc, UC_MIPS_REG_PC, &begin);
        break;
#endif
#ifdef UNICORN_HAS_SPARC
    case UC_ARCH_SPARC:
        // TODO: Sparc/Sparc64
        uc_reg_write(uc, UC_SPARC_REG_PC, &begin);
        break;
#endif
#ifdef UNICORN_HAS_PPC
    case UC_ARCH_PPC:
        uc_reg_write(uc, UC_PPC_REG_PC, &begin);
        break;
#endif
#ifdef UNICORN_HAS_RISCV
    case UC_ARCH_RISCV:
        uc_reg_write(uc, UC_RISCV_REG_PC, &begin);
        break;
#endif
    }

    uc->stop_request = false;

    uc->emu_count = count;
    // remove count hook if counting isn't necessary
    if (count <= 0 && uc->count_hook != 0) {
        uc_hook_del(uc, uc->count_hook);
        uc->count_hook = 0;
    }
    // set up count hook to count instructions.
    if (count > 0 && uc->count_hook == 0) {
        uc_err err;
        // callback to count instructions must be run before everything else,
        // so instead of appending, we must insert the hook at the begin
        // of the hook list
        uc->hook_insert = 1;
        err = uc_hook_add(uc, &uc->count_hook, UC_HOOK_CODE, hook_count_cb,
                          NULL, 1, 0);
        // restore to append mode for uc_hook_add()
        uc->hook_insert = 0;
        if (err != UC_ERR_OK) {
            uc->nested_level--;
            return err;
        }
    }

    // If UC_CTL_UC_USE_EXITS is set, then the @until param won't have any
    // effect. This is designed for the backward compatibility.
    if (!uc->use_exits) {
        g_tree_remove_all(uc->exits);
        uc_add_exit(uc, until);
    }

    if (timeout) {
        enable_emu_timer(uc, timeout * 1000); // microseconds -> nanoseconds
    }

    uc->vm_start(uc);

    // emulation is done
    uc->emulation_done = true;

    // remove hooks to delete
    clear_deleted_hooks(uc);

    if (timeout) {
        // wait for the timer to finish
        qemu_thread_join(&uc->timer);
    }

    uc->nested_level--;
    return uc->invalid_error;
}