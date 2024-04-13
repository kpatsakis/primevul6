static void cpu_unregister_map_client_do(MapClient *client)
{
    QLIST_REMOVE(client, link);
    g_free(client);
}