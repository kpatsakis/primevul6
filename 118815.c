irc_server_reconnect_schedule (struct t_irc_server *server)
{
    int minutes, seconds;

    if (IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_AUTORECONNECT))
    {
        /* growing reconnect delay */
        if (server->reconnect_delay == 0)
            server->reconnect_delay = IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_AUTORECONNECT_DELAY);
        else
            server->reconnect_delay = server->reconnect_delay * weechat_config_integer (irc_config_network_autoreconnect_delay_growing);
        if ((weechat_config_integer (irc_config_network_autoreconnect_delay_max) > 0)
            && (server->reconnect_delay > weechat_config_integer (irc_config_network_autoreconnect_delay_max)))
            server->reconnect_delay = weechat_config_integer (irc_config_network_autoreconnect_delay_max);

        server->reconnect_start = time (NULL);

        minutes = server->reconnect_delay / 60;
        seconds = server->reconnect_delay % 60;
        if ((minutes > 0) && (seconds > 0))
        {
            weechat_printf (
                server->buffer,
                _("%s%s: reconnecting to server in %d %s, %d %s"),
                weechat_prefix ("network"),
                IRC_PLUGIN_NAME,
                minutes,
                NG_("minute", "minutes", minutes),
                seconds,
                NG_("second", "seconds", seconds));
        }
        else if (minutes > 0)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: reconnecting to server in %d %s"),
                weechat_prefix ("network"),
                IRC_PLUGIN_NAME,
                minutes,
                NG_("minute", "minutes", minutes));
        }
        else
        {
            weechat_printf (
                server->buffer,
                _("%s%s: reconnecting to server in %d %s"),
                weechat_prefix ("network"),
                IRC_PLUGIN_NAME,
                seconds,
                NG_("second", "seconds", seconds));
        }
    }
    else
    {
        server->reconnect_delay = 0;
        server->reconnect_start = 0;
    }
}