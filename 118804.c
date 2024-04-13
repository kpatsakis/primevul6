irc_server_add_to_infolist (struct t_infolist *infolist,
                            struct t_irc_server *server)
{
    struct t_infolist_item *ptr_item;

    if (!infolist || !server)
        return 0;

    ptr_item = weechat_infolist_new_item (infolist);
    if (!ptr_item)
        return 0;

    if (!weechat_infolist_new_var_string (ptr_item, "name", server->name))
        return 0;
    if (!weechat_infolist_new_var_pointer (ptr_item, "buffer", server->buffer))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "buffer_name",
                                          (server->buffer) ?
                                          weechat_buffer_get_string (server->buffer, "name") : ""))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "buffer_short_name",
                                          (server->buffer) ?
                                          weechat_buffer_get_string (server->buffer, "short_name") : ""))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "addresses",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_ADDRESSES)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "proxy",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_PROXY)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "ipv6",
                                           IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_IPV6)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "ssl",
                                           IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_SSL)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "ssl_cert",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SSL_CERT)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "ssl_password",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SSL_PASSWORD)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "ssl_priorities",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SSL_PRIORITIES)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "ssl_dhkey_size",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_SSL_DHKEY_SIZE)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "ssl_fingerprint",
                                           IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SSL_FINGERPRINT)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "ssl_verify",
                                           IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_SSL_VERIFY)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "password",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_PASSWORD)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "capabilities",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_CAPABILITIES)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "sasl_mechanism",
                                          IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_SASL_MECHANISM)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "sasl_username",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SASL_USERNAME)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "sasl_password",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SASL_PASSWORD)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "sasl_key",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_SASL_KEY)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "sasl_fail",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_SASL_FAIL)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "autoconnect",
                                           IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_AUTOCONNECT)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "autoreconnect",
                                           IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_AUTORECONNECT)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "autoreconnect_delay",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_AUTORECONNECT_DELAY)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "nicks",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_NICKS)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "nicks_alternate",
                                           IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_NICKS_ALTERNATE)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "username",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_USERNAME)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "realname",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_REALNAME)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "local_hostname",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_LOCAL_HOSTNAME)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "usermode",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_USERMODE)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "command",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_COMMAND)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "command_delay",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_COMMAND_DELAY)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "autojoin",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_AUTOJOIN)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "autorejoin",
                                           IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_AUTOREJOIN)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "autorejoin_delay",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_AUTOREJOIN_DELAY)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "connection_timeout",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_CONNECTION_TIMEOUT)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "anti_flood_prio_high",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_HIGH)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "anti_flood_prio_low",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_LOW)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "away_check",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_AWAY_CHECK)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "away_check_max_nicks",
                                           IRC_SERVER_OPTION_INTEGER(server, IRC_SERVER_OPTION_AWAY_CHECK_MAX_NICKS)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "msg_kick",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_MSG_KICK)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "msg_part",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_MSG_PART)))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "msg_quit",
                                          IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_MSG_QUIT)))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "temp_server", server->temp_server))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "index_current_address", server->index_current_address))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "current_address", server->current_address))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "current_ip", server->current_ip))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "current_port", server->current_port))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "current_retry", server->current_retry))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "sock", server->sock))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "is_connected", server->is_connected))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "ssl_connected", server->ssl_connected))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "disconnected", server->disconnected))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "unterminated_message", server->unterminated_message))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "nick", server->nick))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "nick_modes", server->nick_modes))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "host", server->host))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "checking_cap_ls", server->checking_cap_ls))
        return 0;
    if (!weechat_hashtable_add_to_infolist (server->cap_ls, ptr_item, "cap_ls"))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "checking_cap_list", server->checking_cap_list))
        return 0;
    if (!weechat_hashtable_add_to_infolist (server->cap_list, ptr_item, "cap_list"))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "isupport", server->isupport))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "prefix_modes", server->prefix_modes))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "prefix_chars", server->prefix_chars))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "nick_max_length", server->nick_max_length))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "user_max_length", server->user_max_length))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "host_max_length", server->host_max_length))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "casemapping", server->casemapping))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "casemapping_string", irc_server_casemapping_string[server->casemapping]))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "chantypes", server->chantypes))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "chanmodes", server->chanmodes))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "monitor", server->monitor))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "monitor_time", server->monitor_time))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "reconnect_delay", server->reconnect_delay))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "reconnect_start", server->reconnect_start))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "command_time", server->command_time))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "reconnect_join", server->reconnect_join))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "disable_autojoin", server->disable_autojoin))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "is_away", server->is_away))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "away_message", server->away_message))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "away_time", server->away_time))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "lag", server->lag))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "lag_displayed", server->lag_displayed))
        return 0;
    if (!weechat_infolist_new_var_buffer (ptr_item, "lag_check_time", &(server->lag_check_time), sizeof (struct timeval)))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "lag_next_check", server->lag_next_check))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "lag_last_refresh", server->lag_last_refresh))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "last_user_message", server->last_user_message))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "last_away_check", server->last_away_check))
        return 0;
    if (!weechat_infolist_new_var_time (ptr_item, "last_data_purge", server->last_data_purge))
        return 0;

    return 1;
}