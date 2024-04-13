void helper_uc_tracecode(int32_t size, uc_hook_idx index, void *handle,
                         int64_t address)
{
    struct uc_struct *uc = handle;
    struct list_item *cur;
    struct hook *hook;
    int hook_flags =
        index &
        UC_HOOK_FLAG_MASK; // The index here may contain additional flags. See
                           // the comments of uc_hook_idx for details.

    index = index & UC_HOOK_IDX_MASK;

    // This has been done in tcg code.
    // sync PC in CPUArchState with address
    // if (uc->set_pc) {
    //     uc->set_pc(uc, address);
    // }

    // the last callback may already asked to stop emulation
    if (uc->stop_request && !(hook_flags & UC_HOOK_FLAG_NO_STOP)) {
        return;
    }

    for (cur = uc->hook[index].head;
         cur != NULL && (hook = (struct hook *)cur->data); cur = cur->next) {
        if (hook->to_delete) {
            continue;
        }

        // on invalid block/instruction, call instruction counter (if enable),
        // then quit
        if (size == 0) {
            if (index == UC_HOOK_CODE_IDX && uc->count_hook) {
                // this is the instruction counter (first hook in the list)
                ((uc_cb_hookcode_t)hook->callback)(uc, address, size,
                                                   hook->user_data);
            }

            return;
        }

        if (HOOK_BOUND_CHECK(hook, (uint64_t)address)) {
            ((uc_cb_hookcode_t)hook->callback)(uc, address, size,
                                               hook->user_data);
        }

        // the last callback may already asked to stop emulation
        // Unicorn:
        //   In an ARM IT block, we behave like the emulation continues
        //   normally. No check_exit_request is generated and the hooks are
        //   triggered normally. In other words, the whole IT block is treated
        //   as a single instruction.
        if (uc->stop_request && !(hook_flags & UC_HOOK_FLAG_NO_STOP)) {
            break;
        }
    }
}