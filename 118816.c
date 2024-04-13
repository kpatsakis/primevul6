irc_server_disconnect (struct t_irc_server *server, int switch_address,
                       int reconnect)
{
    struct t_irc_channel *ptr_channel;

    if (server->is_connected)
    {
        /*
         * remove all nicks and write disconnection message on each
         * channel/private buffer
         */
        for (ptr_channel = server->channels; ptr_channel;
             ptr_channel = ptr_channel->next_channel)
        {
            irc_nick_free_all (server, ptr_channel);
            if (ptr_channel->hook_autorejoin)
            {
                weechat_unhook (ptr_channel->hook_autorejoin);
                ptr_channel->hook_autorejoin = NULL;
            }
            weechat_buffer_set (ptr_channel->buffer, "localvar_del_away", "");
            weechat_printf (
                ptr_channel->buffer,
                _("%s%s: disconnected from server"),
                weechat_prefix ("network"), IRC_PLUGIN_NAME);
        }
        /* remove away status on server buffer */
        weechat_buffer_set (server->buffer, "localvar_del_away", "");
    }

    irc_server_close_connection (server);

    if (server->buffer)
    {
        weechat_printf (
            server->buffer,
            _("%s%s: disconnected from server"),
            weechat_prefix ("network"), IRC_PLUGIN_NAME);
    }

    server->current_retry = 0;

    if (switch_address)
        irc_server_switch_address (server, 0);
    else
        irc_server_set_index_current_address (server, 0);

    if (server->nick_modes)
    {
        free (server->nick_modes);
        server->nick_modes = NULL;
        weechat_bar_item_update ("input_prompt");
        weechat_bar_item_update ("irc_nick_modes");
    }
    if (server->host)
    {
        free (server->host);
        server->host = NULL;
        weechat_bar_item_update ("irc_host");
        weechat_bar_item_update ("irc_nick_host");
    }
    server->checking_cap_ls = 0;
    weechat_hashtable_remove_all (server->cap_ls);
    server->checking_cap_list = 0;
    weechat_hashtable_remove_all (server->cap_list);
    server->is_away = 0;
    server->away_time = 0;
    server->lag = 0;
    server->lag_displayed = -1;
    server->lag_check_time.tv_sec = 0;
    server->lag_check_time.tv_usec = 0;
    server->lag_next_check = time (NULL) +
        weechat_config_integer (irc_config_network_lag_check);
    server->lag_last_refresh = 0;
    irc_server_set_lag (server);
    server->monitor = 0;
    server->monitor_time = 0;

    if (reconnect
        && IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_AUTORECONNECT))
        irc_server_reconnect_schedule (server);
    else
    {
        server->reconnect_delay = 0;
        server->reconnect_start = 0;
    }

    /* discard current nick if no reconnection asked */
    if (!reconnect && server->nick)
        irc_server_set_nick (server, NULL);

    irc_server_set_buffer_title (server);

    server->disconnected = 1;

    /* send signal "irc_server_disconnected" with server name */
    (void) weechat_hook_signal_send ("irc_server_disconnected",
                                     WEECHAT_HOOK_SIGNAL_STRING, server->name);
}