static void hook_count_cb(struct uc_struct *uc, uint64_t address, uint32_t size,
                          void *user_data)
{
    // count this instruction. ah ah ah.
    uc->emu_counter++;
    // printf(":: emu counter = %u, at %lx\n", uc->emu_counter, address);

    if (uc->emu_counter > uc->emu_count) {
        // printf(":: emu counter = %u, stop emulation\n", uc->emu_counter);
        uc_emu_stop(uc);
    }
}