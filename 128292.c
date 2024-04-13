void qemu_mutex_unlock_ramlist(void)
{
    qemu_mutex_unlock(&ram_list.mutex);
}