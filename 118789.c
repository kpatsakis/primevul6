irc_server_reconnect (struct t_irc_server *server)
{
    weechat_printf (
        server->buffer,
        _("%s%s: reconnecting to server..."),
        weechat_prefix ("network"), IRC_PLUGIN_NAME);

    server->reconnect_start = 0;

    if (irc_server_connect (server))
        server->reconnect_join = 1;
    else
        irc_server_reconnect_schedule (server);
}