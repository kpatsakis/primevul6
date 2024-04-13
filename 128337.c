static void cpu_notify_map_clients(void)
{
    qemu_mutex_lock(&map_client_list_lock);
    cpu_notify_map_clients_locked();
    qemu_mutex_unlock(&map_client_list_lock);
}