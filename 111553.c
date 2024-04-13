uc_err uc_hook_add(uc_engine *uc, uc_hook *hh, int type, void *callback,
                   void *user_data, uint64_t begin, uint64_t end, ...)
{
    int ret = UC_ERR_OK;
    int i = 0;

    UC_INIT(uc);

    struct hook *hook = calloc(1, sizeof(struct hook));
    if (hook == NULL) {
        return UC_ERR_NOMEM;
    }

    hook->begin = begin;
    hook->end = end;
    hook->type = type;
    hook->callback = callback;
    hook->user_data = user_data;
    hook->refs = 0;
    hook->to_delete = false;
    *hh = (uc_hook)hook;

    // UC_HOOK_INSN has an extra argument for instruction ID
    if (type & UC_HOOK_INSN) {
        va_list valist;

        va_start(valist, end);
        hook->insn = va_arg(valist, int);
        va_end(valist);

        if (uc->insn_hook_validate) {
            if (!uc->insn_hook_validate(hook->insn)) {
                free(hook);
                return UC_ERR_HOOK;
            }
        }

        if (uc->hook_insert) {
            if (list_insert(&uc->hook[UC_HOOK_INSN_IDX], hook) == NULL) {
                free(hook);
                return UC_ERR_NOMEM;
            }
        } else {
            if (list_append(&uc->hook[UC_HOOK_INSN_IDX], hook) == NULL) {
                free(hook);
                return UC_ERR_NOMEM;
            }
        }

        uc->hooks_count[UC_HOOK_INSN_IDX]++;
        hook->refs++;
        return UC_ERR_OK;
    }

    if (type & UC_HOOK_TCG_OPCODE) {
        va_list valist;

        va_start(valist, end);
        hook->op = va_arg(valist, int);
        hook->op_flags = va_arg(valist, int);
        va_end(valist);

        if (uc->opcode_hook_invalidate) {
            if (!uc->opcode_hook_invalidate(hook->op, hook->op_flags)) {
                free(hook);
                return UC_ERR_HOOK;
            }
        }

        if (uc->hook_insert) {
            if (list_insert(&uc->hook[UC_HOOK_TCG_OPCODE_IDX], hook) == NULL) {
                free(hook);
                return UC_ERR_NOMEM;
            }
        } else {
            if (list_append(&uc->hook[UC_HOOK_TCG_OPCODE_IDX], hook) == NULL) {
                free(hook);
                return UC_ERR_NOMEM;
            }
        }

        uc->hooks_count[UC_HOOK_TCG_OPCODE_IDX]++;
        hook->refs++;
        return UC_ERR_OK;
    }

    while ((type >> i) > 0) {
        if ((type >> i) & 1) {
            // TODO: invalid hook error?
            if (i < UC_HOOK_MAX) {
                if (uc->hook_insert) {
                    if (list_insert(&uc->hook[i], hook) == NULL) {
                        if (hook->refs == 0) {
                            free(hook);
                        }
                        return UC_ERR_NOMEM;
                    }
                } else {
                    if (list_append(&uc->hook[i], hook) == NULL) {
                        if (hook->refs == 0) {
                            free(hook);
                        }
                        return UC_ERR_NOMEM;
                    }
                }
                uc->hooks_count[i]++;
                hook->refs++;
            }
        }
        i++;
    }

    // we didn't use the hook
    // TODO: return an error?
    if (hook->refs == 0) {
        free(hook);
    }

    return ret;
}