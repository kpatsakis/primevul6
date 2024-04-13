irc_server_auto_connect_timer_cb (const void *pointer, void *data,
                                  int remaining_calls)
{
    struct t_irc_server *ptr_server;
    int auto_connect;

    /* make C compiler happy */
    (void) data;
    (void) remaining_calls;

    auto_connect = (pointer) ? 1 : 0;

    for (ptr_server = irc_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        if ((auto_connect || ptr_server->temp_server)
            && (IRC_SERVER_OPTION_BOOLEAN(ptr_server, IRC_SERVER_OPTION_AUTOCONNECT)))
        {
            if (!irc_server_connect (ptr_server))
                irc_server_reconnect_schedule (ptr_server);
        }
    }

    return WEECHAT_RC_OK;
}