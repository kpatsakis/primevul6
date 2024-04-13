irc_server_free (struct t_irc_server *server)
{
    struct t_irc_server *new_irc_servers;

    if (!server)
        return;

    /*
     * close server buffer (and all channels/privates)
     * (only if we are not in a /upgrade, because during upgrade we want to
     * keep connections and closing server buffer would disconnect from server)
     */
    if (server->buffer && !irc_signal_upgrade_received)
        weechat_buffer_close (server->buffer);

    /* remove server from queue */
    if (last_irc_server == server)
        last_irc_server = server->prev_server;
    if (server->prev_server)
    {
        (server->prev_server)->next_server = server->next_server;
        new_irc_servers = irc_servers;
    }
    else
        new_irc_servers = server->next_server;

    if (server->next_server)
        (server->next_server)->prev_server = server->prev_server;

    irc_server_free_data (server);
    free (server);
    irc_servers = new_irc_servers;
}