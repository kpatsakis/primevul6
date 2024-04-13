void helper_uc_traceopcode(struct hook *hook, uint64_t arg1, uint64_t arg2,
                           uint32_t size, void *handle, uint64_t address)
{
    struct uc_struct *uc = handle;

    if (unlikely(uc->stop_request)) {
        return;
    }

    if (unlikely(hook->to_delete)) {
        return;
    }

    // We did all checks in translation time.
    //
    // This could optimize the case that we have multiple hooks with different
    // opcodes and have one callback per opcode. Note that the assumption don't
    // hold in most cases for uc_tracecode.
    //
    // TODO: Shall we have a flag to allow users to control whether updating PC?
    ((uc_hook_tcg_op_2)hook->callback)(uc, address, arg1, arg2, size,
                                       hook->user_data);

    if (unlikely(uc->stop_request)) {
        return;
    }
}