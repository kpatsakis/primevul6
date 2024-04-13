irc_server_outqueue_free_all (struct t_irc_server *server, int priority)
{
    while (server->outqueue[priority])
    {
        irc_server_outqueue_free (server, priority,
                                  server->outqueue[priority]);
    }
}