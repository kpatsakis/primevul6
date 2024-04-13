irc_server_valid (struct t_irc_server *server)
{
    struct t_irc_server *ptr_server;

    if (!server)
        return 0;

    for (ptr_server = irc_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        if (ptr_server == server)
            return 1;
    }

    /* server not found */
    return 0;
}