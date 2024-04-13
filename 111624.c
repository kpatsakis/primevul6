uc_err uc_ctl(uc_engine *uc, uc_control_type control, ...)
{
    int rw, type;
    uc_err err = UC_ERR_OK;
    va_list args;

    // MSVC Would do signed shift on signed integers.
    rw = (uint32_t)control >> 30;
    type = (control & ((1 << 16) - 1));
    va_start(args, control);

    switch (type) {
    case UC_CTL_UC_MODE: {
        if (rw == UC_CTL_IO_READ) {
            int *pmode = va_arg(args, int *);
            *pmode = uc->mode;
        } else {
            err = UC_ERR_ARG;
        }
        break;
    }

    case UC_CTL_UC_ARCH: {
        if (rw == UC_CTL_IO_READ) {
            int *arch = va_arg(args, int *);
            *arch = uc->arch;
        } else {
            err = UC_ERR_ARG;
        }
        break;
    }

    case UC_CTL_UC_TIMEOUT: {
        if (rw == UC_CTL_IO_READ) {
            uint64_t *arch = va_arg(args, uint64_t *);
            *arch = uc->timeout;
        } else {
            err = UC_ERR_ARG;
        }
        break;
    }

    case UC_CTL_UC_PAGE_SIZE: {
        if (rw == UC_CTL_IO_READ) {

            UC_INIT(uc);

            uint32_t *page_size = va_arg(args, uint32_t *);
            *page_size = uc->target_page_size;
        } else {
            uint32_t page_size = va_arg(args, uint32_t);
            int bits = 0;

            if (uc->init_done) {
                err = UC_ERR_ARG;
                break;
            }

            if (uc->arch != UC_ARCH_ARM) {
                err = UC_ERR_ARG;
                break;
            }

            if ((page_size & (page_size - 1))) {
                err = UC_ERR_ARG;
                break;
            }

            while (page_size) {
                bits++;
                page_size >>= 1;
            }

            uc->target_bits = bits;

            err = UC_ERR_OK;
        }
        break;
    }

    case UC_CTL_UC_USE_EXITS: {
        if (rw == UC_CTL_IO_WRITE) {
            int use_exits = va_arg(args, int);
            uc->use_exits = use_exits;
        } else {
            err = UC_ERR_ARG;
        }
        break;
    }

    case UC_CTL_UC_EXITS_CNT: {

        UC_INIT(uc);

        if (!uc->use_exits) {
            err = UC_ERR_ARG;
        } else if (rw == UC_CTL_IO_READ) {
            size_t *exits_cnt = va_arg(args, size_t *);
            *exits_cnt = g_tree_nnodes(uc->exits);
        } else {
            err = UC_ERR_ARG;
        }
        break;
    }

    case UC_CTL_UC_EXITS: {

        UC_INIT(uc);

        if (!uc->use_exits) {
            err = UC_ERR_ARG;
        } else if (rw == UC_CTL_IO_READ) {
            uint64_t *exits = va_arg(args, uint64_t *);
            size_t cnt = va_arg(args, size_t);
            if (cnt < g_tree_nnodes(uc->exits)) {
                err = UC_ERR_ARG;
            } else {
                uc_ctl_exit_request req;
                req.array = exits;
                req.len = 0;

                g_tree_foreach(uc->exits, uc_read_exit_iter, (void *)&req);
            }
        } else if (rw == UC_CTL_IO_WRITE) {
            uint64_t *exits = va_arg(args, uint64_t *);
            size_t cnt = va_arg(args, size_t);

            g_tree_remove_all(uc->exits);

            for (size_t i = 0; i < cnt; i++) {
                uc_add_exit(uc, exits[i]);
            }
        } else {
            err = UC_ERR_ARG;
        }
        break;
    }

    case UC_CTL_CPU_MODEL: {
        if (rw == UC_CTL_IO_READ) {

            UC_INIT(uc);

            int *model = va_arg(args, int *);
            *model = uc->cpu_model;
        } else {
            int model = va_arg(args, int);

            if (uc->init_done) {
                err = UC_ERR_ARG;
                break;
            }

            uc->cpu_model = model;

            err = UC_ERR_OK;
        }
        break;
    }

    case UC_CTL_TB_REQUEST_CACHE: {

        UC_INIT(uc);

        if (rw == UC_CTL_IO_READ_WRITE) {
            uint64_t addr = va_arg(args, uint64_t);
            uc_tb *tb = va_arg(args, uc_tb *);
            err = uc->uc_gen_tb(uc, addr, tb);
        } else {
            err = UC_ERR_ARG;
        }
        break;
    }

    case UC_CTL_TB_REMOVE_CACHE: {

        UC_INIT(uc);

        if (rw == UC_CTL_IO_WRITE) {
            uint64_t addr = va_arg(args, uint64_t);
            uint64_t end = va_arg(args, uint64_t);
            if (end <= addr) {
                err = UC_ERR_ARG;
            } else {
                uc->uc_invalidate_tb(uc, addr, end - addr);
            }
        } else {
            err = UC_ERR_ARG;
        }
        break;
    }

    default:
        err = UC_ERR_ARG;
        break;
    }

    va_end(args);

    return err;
}