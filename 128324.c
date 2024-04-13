void cpu_unregister_map_client(QEMUBH *bh)
{
    MapClient *client;

    qemu_mutex_lock(&map_client_list_lock);
    QLIST_FOREACH(client, &map_client_list, link) {
        if (client->bh == bh) {
            cpu_unregister_map_client_do(client);
            break;
        }
    }
    qemu_mutex_unlock(&map_client_list_lock);
}