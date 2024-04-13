irc_server_timer_connection_cb (const void *pointer, void *data,
                                int remaining_calls)
{
    struct t_irc_server *server;

    /* make C compiler happy */
    (void) data;
    (void) remaining_calls;

    server = (struct t_irc_server *)pointer;

    if (!server)
        return WEECHAT_RC_ERROR;

    server->hook_timer_connection = NULL;

    if (!server->is_connected)
    {
        weechat_printf (
            server->buffer,
            _("%s%s: connection timeout (message 001 not received)"),
            weechat_prefix ("error"), IRC_PLUGIN_NAME);
        irc_server_disconnect (server, !server->is_connected, 1);
    }

    return WEECHAT_RC_OK;
}