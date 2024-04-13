irc_server_autojoin_channels (struct t_irc_server *server)
{
    struct t_irc_channel *ptr_channel;
    char *autojoin;

    /* auto-join after disconnection (only rejoins opened channels) */
    if (!server->disable_autojoin && server->reconnect_join && server->channels)
    {
        for (ptr_channel = server->channels; ptr_channel;
             ptr_channel = ptr_channel->next_channel)
        {
            if ((ptr_channel->type == IRC_CHANNEL_TYPE_CHANNEL)
                && !ptr_channel->part)
            {
                if (ptr_channel->key)
                {
                    irc_server_sendf (server,
                                      IRC_SERVER_SEND_OUTQ_PRIO_HIGH, NULL,
                                      "JOIN %s %s",
                                      ptr_channel->name, ptr_channel->key);
                }
                else
                {
                    irc_server_sendf (server,
                                      IRC_SERVER_SEND_OUTQ_PRIO_HIGH, NULL,
                                      "JOIN %s",
                                      ptr_channel->name);
                }
            }
        }
        server->reconnect_join = 0;
    }
    else
    {
        /* auto-join when connecting to server for first time */
        autojoin = irc_server_eval_expression (
            server,
            IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_AUTOJOIN));
        if (!server->disable_autojoin && autojoin && autojoin[0])
            irc_command_join_server (server, autojoin, 0, 0);
        if (autojoin)
            free (autojoin);
    }

    server->disable_autojoin = 0;
}