irc_server_search (const char *server_name)
{
    struct t_irc_server *ptr_server;

    if (!server_name)
        return NULL;

    for (ptr_server = irc_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        if (strcmp (ptr_server->name, server_name) == 0)
            return ptr_server;
    }

    /* server not found */
    return NULL;
}