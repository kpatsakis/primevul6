irc_server_timer_cb (const void *pointer, void *data, int remaining_calls)
{
    struct t_irc_server *ptr_server;
    struct t_irc_channel *ptr_channel;
    struct t_irc_redirect *ptr_redirect, *ptr_next_redirect;
    time_t current_time;
    static struct timeval tv;
    int away_check, refresh_lag;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) remaining_calls;

    current_time = time (NULL);

    for (ptr_server = irc_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        /* check if reconnection is pending */
        if ((!ptr_server->is_connected)
            && (ptr_server->reconnect_start > 0)
            && (current_time >= (ptr_server->reconnect_start + ptr_server->reconnect_delay)))
        {
            irc_server_reconnect (ptr_server);
        }
        else
        {
            if (!ptr_server->is_connected)
                continue;

            /* send queued messages */
            irc_server_outqueue_send (ptr_server);

            /* check for lag */
            if ((weechat_config_integer (irc_config_network_lag_check) > 0)
                && (ptr_server->lag_check_time.tv_sec == 0)
                && (current_time >= ptr_server->lag_next_check))
            {
                irc_server_sendf (ptr_server, 0, NULL, "PING %s",
                                  (ptr_server->current_address) ?
                                  ptr_server->current_address : "weechat");
                gettimeofday (&(ptr_server->lag_check_time), NULL);
                ptr_server->lag = 0;
                ptr_server->lag_last_refresh = 0;
            }
            else
            {
                /* check away (only if lag check was not done) */
                away_check = IRC_SERVER_OPTION_INTEGER(
                    ptr_server, IRC_SERVER_OPTION_AWAY_CHECK);
                if (!weechat_hashtable_has_key (ptr_server->cap_list,
                                                "away-notify")
                    && (away_check > 0)
                    && ((ptr_server->last_away_check == 0)
                        || (current_time >= ptr_server->last_away_check + (away_check * 60))))
                {
                    irc_server_check_away (ptr_server);
                }
            }

            /* check if it's time to autojoin channels (after command delay) */
            if ((ptr_server->command_time != 0)
                && (current_time >= ptr_server->command_time +
                    IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_COMMAND_DELAY)))
            {
                irc_server_autojoin_channels (ptr_server);
                ptr_server->command_time = 0;
            }

            /* check if it's time to send MONITOR command */
            if ((ptr_server->monitor_time != 0)
                && (current_time >= ptr_server->monitor_time))
            {
                if (ptr_server->monitor > 0)
                    irc_notify_send_monitor (ptr_server);
                ptr_server->monitor_time = 0;
            }

            /* compute lag */
            if (ptr_server->lag_check_time.tv_sec != 0)
            {
                refresh_lag = 0;
                gettimeofday (&tv, NULL);
                ptr_server->lag = (int)(weechat_util_timeval_diff (&(ptr_server->lag_check_time),
                                                                   &tv) / 1000);
                /* refresh lag item if needed */
                if (((ptr_server->lag_last_refresh == 0)
                     || (current_time >= ptr_server->lag_last_refresh + weechat_config_integer (irc_config_network_lag_refresh_interval)))
                    && (ptr_server->lag >= weechat_config_integer (irc_config_network_lag_min_show)))
                {
                    ptr_server->lag_last_refresh = current_time;
                    if (ptr_server->lag != ptr_server->lag_displayed)
                    {
                        ptr_server->lag_displayed = ptr_server->lag;
                        refresh_lag = 1;
                    }
                }
                /* lag timeout? => disconnect */
                if ((weechat_config_integer (irc_config_network_lag_reconnect) > 0)
                    && (ptr_server->lag >= weechat_config_integer (irc_config_network_lag_reconnect) * 1000))
                {
                    weechat_printf (
                        ptr_server->buffer,
                        _("%s%s: lag is high, reconnecting to server %s%s%s"),
                        weechat_prefix ("network"),
                        IRC_PLUGIN_NAME,
                        IRC_COLOR_CHAT_SERVER,
                        ptr_server->name,
                        IRC_COLOR_RESET);
                    irc_server_disconnect (ptr_server, 0, 1);
                }
                else
                {
                    /* stop lag counting if max lag is reached */
                    if ((weechat_config_integer (irc_config_network_lag_max) > 0)
                        && (ptr_server->lag >= (weechat_config_integer (irc_config_network_lag_max) * 1000)))
                    {
                        /* refresh lag item */
                        ptr_server->lag_last_refresh = current_time;
                        if (ptr_server->lag != ptr_server->lag_displayed)
                        {
                            ptr_server->lag_displayed = ptr_server->lag;
                            refresh_lag = 1;
                        }

                        /* schedule next lag check in 5 seconds */
                        ptr_server->lag_check_time.tv_sec = 0;
                        ptr_server->lag_check_time.tv_usec = 0;
                        ptr_server->lag_next_check = time (NULL) +
                            weechat_config_integer (irc_config_network_lag_check);
                    }
                }
                if (refresh_lag)
                    irc_server_set_lag (ptr_server);
            }

            /* remove redirects if timeout occurs */
            ptr_redirect = ptr_server->redirects;
            while (ptr_redirect)
            {
                ptr_next_redirect = ptr_redirect->next_redirect;

                if ((ptr_redirect->start_time > 0)
                    && (ptr_redirect->start_time + ptr_redirect->timeout < current_time))
                {
                    irc_redirect_stop (ptr_redirect, "timeout");
                }

                ptr_redirect = ptr_next_redirect;
            }

            /* purge some data (every 10 minutes) */
            if (current_time > ptr_server->last_data_purge + (60 * 10))
            {
                weechat_hashtable_map (ptr_server->join_manual,
                                       &irc_server_check_join_manual_cb,
                                       NULL);
                weechat_hashtable_map (ptr_server->join_noswitch,
                                       &irc_server_check_join_noswitch_cb,
                                       NULL);
                for (ptr_channel = ptr_server->channels; ptr_channel;
                     ptr_channel = ptr_channel->next_channel)
                {
                    if (ptr_channel->join_smart_filtered)
                    {
                        weechat_hashtable_map (ptr_channel->join_smart_filtered,
                                               &irc_server_check_join_smart_filtered_cb,
                                               NULL);
                    }
                }
                ptr_server->last_data_purge = current_time;
            }
        }
    }

    return WEECHAT_RC_OK;
}