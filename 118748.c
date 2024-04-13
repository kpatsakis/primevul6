irc_server_set_lag (struct t_irc_server *server)
{
    char str_lag[32];

    if (server->lag >= weechat_config_integer (irc_config_network_lag_min_show))
    {
        snprintf (str_lag, sizeof (str_lag),
                  ((server->lag_check_time.tv_sec == 0) || (server->lag < 1000)) ?
                  "%.3f" : "%.0f",
                  ((float)(server->lag)) / 1000);
        weechat_buffer_set (server->buffer, "localvar_set_lag", str_lag);
    }
    else
    {
        weechat_buffer_set (server->buffer, "localvar_del_lag", "");
    }
    weechat_hook_signal_send ("irc_server_lag_changed",
                              WEECHAT_HOOK_SIGNAL_STRING,
                              server->name);
    weechat_bar_item_update ("lag");
}