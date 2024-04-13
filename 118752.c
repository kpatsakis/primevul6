irc_server_switch_address (struct t_irc_server *server, int connection)
{
    if (server->addresses_count > 1)
    {
        irc_server_set_index_current_address (
            server,
            (server->index_current_address + 1) % server->addresses_count);
        weechat_printf (
            server->buffer,
            _("%s%s: switching address to %s/%d"),
            weechat_prefix ("network"),
            IRC_PLUGIN_NAME,
            server->current_address,
            server->current_port);
        if (connection)
        {
            if (server->index_current_address == 0)
                irc_server_reconnect_schedule (server);
            else
                irc_server_connect (server);
        }
    }
    else
    {
        if (connection)
            irc_server_reconnect_schedule (server);
    }
}