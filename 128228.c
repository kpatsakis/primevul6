void cpu_register_map_client(QEMUBH *bh)
{
    MapClient *client = g_malloc(sizeof(*client));

    qemu_mutex_lock(&map_client_list_lock);
    client->bh = bh;
    QLIST_INSERT_HEAD(&map_client_list, client, link);
    if (!atomic_read(&bounce.in_use)) {
        cpu_notify_map_clients_locked();
    }
    qemu_mutex_unlock(&map_client_list_lock);
}