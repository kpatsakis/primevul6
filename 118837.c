irc_server_timer_sasl_cb (const void *pointer, void *data, int remaining_calls)
{
    struct t_irc_server *server;
    int sasl_fail;

    /* make C compiler happy */
    (void) data;
    (void) remaining_calls;

    server = (struct t_irc_server *)pointer;

    if (!server)
        return WEECHAT_RC_ERROR;

    server->hook_timer_sasl = NULL;

    if (!server->is_connected)
    {
        weechat_printf (server->buffer,
                        _("%s%s: SASL authentication timeout"),
                        weechat_prefix ("error"), IRC_PLUGIN_NAME);
        sasl_fail = IRC_SERVER_OPTION_INTEGER(server,
                                              IRC_SERVER_OPTION_SASL_FAIL);
        if ((sasl_fail == IRC_SERVER_SASL_FAIL_RECONNECT)
            || (sasl_fail == IRC_SERVER_SASL_FAIL_DISCONNECT))
        {
            irc_server_disconnect (
                server, 0,
                (sasl_fail == IRC_SERVER_SASL_FAIL_RECONNECT) ? 1 : 0);
        }
        else
            irc_server_sendf (server, 0, NULL, "CAP END");
    }

    return WEECHAT_RC_OK;
}