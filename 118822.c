irc_server_hdata_server_cb (const void *pointer, void *data,
                            const char *hdata_name)
{
    struct t_hdata *hdata;

    /* make C compiler happy */
    (void) pointer;
    (void) data;

    hdata = weechat_hdata_new (hdata_name, "prev_server", "next_server",
                               0, 0, NULL, NULL);
    if (hdata)
    {
        WEECHAT_HDATA_VAR(struct t_irc_server, name, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, options, POINTER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, temp_server, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, reloading_from_config, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, reloaded_from_config, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, addresses_eval, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, addresses_count, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, addresses_array, STRING, 0, "addresses_count", NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, ports_array, INTEGER, 0, "addresses_count", NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, retry_array, INTEGER, 0, "addresses_count", NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, index_current_address, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, current_address, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, current_ip, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, current_port, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, current_retry, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, sock, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, hook_connect, POINTER, 0, NULL, "hook");
        WEECHAT_HDATA_VAR(struct t_irc_server, hook_fd, POINTER, 0, NULL, "hook");
        WEECHAT_HDATA_VAR(struct t_irc_server, hook_timer_connection, POINTER, 0, NULL, "hook");
        WEECHAT_HDATA_VAR(struct t_irc_server, hook_timer_sasl, POINTER, 0, NULL, "hook");
        WEECHAT_HDATA_VAR(struct t_irc_server, is_connected, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, ssl_connected, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, disconnected, INTEGER, 0, NULL, NULL);
#ifdef HAVE_GNUTLS
        WEECHAT_HDATA_VAR(struct t_irc_server, gnutls_sess, OTHER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, tls_cert, OTHER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, tls_cert_key, OTHER, 0, NULL, NULL);
#endif /* HAVE_GNUTLS */
        WEECHAT_HDATA_VAR(struct t_irc_server, unterminated_message, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, nicks_count, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, nicks_array, STRING, 0, "nicks_count", NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, nick_first_tried, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, nick_alternate_number, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, nick, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, nick_modes, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, host, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, checking_cap_ls, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, cap_ls, HASHTABLE, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, checking_cap_list, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, cap_list, HASHTABLE, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, isupport, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, prefix_modes, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, prefix_chars, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, nick_max_length, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, user_max_length, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, host_max_length, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, casemapping, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, chantypes, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, chanmodes, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, monitor, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, monitor_time, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, reconnect_delay, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, reconnect_start, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, command_time, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, reconnect_join, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, disable_autojoin, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, is_away, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, away_message, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, away_time, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, lag, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, lag_displayed, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, lag_check_time, OTHER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, lag_next_check, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, lag_last_refresh, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, cmd_list_regexp, POINTER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, last_user_message, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, last_away_check, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, last_data_purge, TIME, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, outqueue, POINTER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, last_outqueue, POINTER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, redirects, POINTER, 0, NULL, "irc_redirect");
        WEECHAT_HDATA_VAR(struct t_irc_server, last_redirect, POINTER, 0, NULL, "irc_redirect");
        WEECHAT_HDATA_VAR(struct t_irc_server, notify_list, POINTER, 0, NULL, "irc_notify");
        WEECHAT_HDATA_VAR(struct t_irc_server, last_notify, POINTER, 0, NULL, "irc_notify");
        WEECHAT_HDATA_VAR(struct t_irc_server, notify_count, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, join_manual, HASHTABLE, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, join_channel_key, HASHTABLE, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, join_noswitch, HASHTABLE, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, buffer, POINTER, 0, NULL, "buffer");
        WEECHAT_HDATA_VAR(struct t_irc_server, buffer_as_string, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_server, channels, POINTER, 0, NULL, "irc_channel");
        WEECHAT_HDATA_VAR(struct t_irc_server, last_channel, POINTER, 0, NULL, "irc_channel");
        WEECHAT_HDATA_VAR(struct t_irc_server, prev_server, POINTER, 0, NULL, hdata_name);
        WEECHAT_HDATA_VAR(struct t_irc_server, next_server, POINTER, 0, NULL, hdata_name);
        WEECHAT_HDATA_LIST(irc_servers, WEECHAT_HDATA_LIST_CHECK_POINTERS);
        WEECHAT_HDATA_LIST(last_irc_server, 0);
    }
    return hdata;
}