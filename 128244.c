void qemu_mutex_lock_ramlist(void)
{
    qemu_mutex_lock(&ram_list.mutex);
}