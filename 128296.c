static bool prepare_mmio_access(MemoryRegion *mr)
{
    bool unlocked = !qemu_mutex_iothread_locked();
    bool release_lock = false;

    if (unlocked && mr->global_locking) {
        qemu_mutex_lock_iothread();
        unlocked = false;
        release_lock = true;
    }
    if (mr->flush_coalesced_mmio) {
        if (unlocked) {
            qemu_mutex_lock_iothread();
        }
        qemu_flush_coalesced_mmio_buffer();
        if (unlocked) {
            qemu_mutex_unlock_iothread();
        }
    }

    return release_lock;
}