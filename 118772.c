irc_server_print_log ()
{
    struct t_irc_server *ptr_server;
    struct t_irc_channel *ptr_channel;
    int i;

    for (ptr_server = irc_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        weechat_log_printf ("");
        weechat_log_printf ("[server %s (addr:0x%lx)]", ptr_server->name, ptr_server);
        /* addresses */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_ADDRESSES]))
            weechat_log_printf ("  addresses. . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_ADDRESSES));
        else
            weechat_log_printf ("  addresses. . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_ADDRESSES]));
        /* proxy */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_PROXY]))
            weechat_log_printf ("  proxy. . . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_PROXY));
        else
            weechat_log_printf ("  proxy. . . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_PROXY]));
        /* ipv6 */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_IPV6]))
            weechat_log_printf ("  ipv6 . . . . . . . . : null (%s)",
                                (IRC_SERVER_OPTION_BOOLEAN(ptr_server, IRC_SERVER_OPTION_IPV6)) ?
                                "on" : "off");
        else
            weechat_log_printf ("  ipv6 . . . . . . . . : %s",
                                (weechat_config_boolean (ptr_server->options[IRC_SERVER_OPTION_IPV6])) ?
                                "on" : "off");
        /* ssl */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SSL]))
            weechat_log_printf ("  ssl. . . . . . . . . : null (%s)",
                                (IRC_SERVER_OPTION_BOOLEAN(ptr_server, IRC_SERVER_OPTION_SSL)) ?
                                "on" : "off");
        else
            weechat_log_printf ("  ssl. . . . . . . . . : %s",
                                (weechat_config_boolean (ptr_server->options[IRC_SERVER_OPTION_SSL])) ?
                                "on" : "off");
        /* ssl_cert */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SSL_CERT]))
            weechat_log_printf ("  ssl_cert . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_SSL_CERT));
        else
            weechat_log_printf ("  ssl_cert . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_SSL_CERT]));
        /* ssl_password */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SSL_PASSWORD]))
            weechat_log_printf ("  ssl_password . . . . : null");
        else
            weechat_log_printf ("  ssl_password . . . . : (hidden)");
        /* ssl_priorities */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SSL_PRIORITIES]))
            weechat_log_printf ("  ssl_priorities . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_SSL_PRIORITIES));
        else
            weechat_log_printf ("  ssl_priorities . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_SSL_PRIORITIES]));
        /* ssl_dhkey_size */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SSL_DHKEY_SIZE]))
            weechat_log_printf ("  ssl_dhkey_size . . . : null ('%d')",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_SSL_DHKEY_SIZE));
        else
            weechat_log_printf ("  ssl_dhkey_size . . . : '%d'",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_SSL_DHKEY_SIZE]));
        /* ssl_fingerprint */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SSL_FINGERPRINT]))
            weechat_log_printf ("  ssl_fingerprint. . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_SSL_FINGERPRINT));
        else
            weechat_log_printf ("  ssl_fingerprint. . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_SSL_FINGERPRINT]));
        /* ssl_verify */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SSL_VERIFY]))
            weechat_log_printf ("  ssl_verify . . . . . : null (%s)",
                                (IRC_SERVER_OPTION_BOOLEAN(ptr_server, IRC_SERVER_OPTION_SSL_VERIFY)) ?
                                "on" : "off");
        else
            weechat_log_printf ("  ssl_verify . . . . . : %s",
                                (weechat_config_boolean (ptr_server->options[IRC_SERVER_OPTION_SSL_VERIFY])) ?
                                "on" : "off");
        /* password */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_PASSWORD]))
            weechat_log_printf ("  password . . . . . . : null");
        else
            weechat_log_printf ("  password . . . . . . : (hidden)");
        /* client capabilities */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_CAPABILITIES]))
            weechat_log_printf ("  capabilities . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_CAPABILITIES));
        else
            weechat_log_printf ("  capabilities . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_CAPABILITIES]));
        /* sasl_mechanism */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SASL_MECHANISM]))
            weechat_log_printf ("  sasl_mechanism . . . : null ('%s')",
                                irc_sasl_mechanism_string[IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_SASL_MECHANISM)]);
        else
            weechat_log_printf ("  sasl_mechanism . . . : '%s'",
                                irc_sasl_mechanism_string[weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_SASL_MECHANISM])]);
        /* sasl_username */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SASL_USERNAME]))
            weechat_log_printf ("  sasl_username. . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_SASL_USERNAME));
        else
            weechat_log_printf ("  sasl_username. . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_SASL_USERNAME]));
        /* sasl_password */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SASL_PASSWORD]))
            weechat_log_printf ("  sasl_password. . . . : null");
        else
            weechat_log_printf ("  sasl_password. . . . : (hidden)");
        /* sasl_key */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SASL_KEY]))
            weechat_log_printf ("  sasl_key. .  . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_SASL_KEY));
        else
            weechat_log_printf ("  sasl_key. .  . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_SASL_KEY]));
        /* sasl_fail */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_SASL_FAIL]))
            weechat_log_printf ("  sasl_fail. . . . . . : null ('%s')",
                                irc_server_sasl_fail_string[IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_SASL_FAIL)]);
        else
            weechat_log_printf ("  sasl_fail. . . . . . : '%s'",
                                irc_server_sasl_fail_string[weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_SASL_FAIL])]);
        /* autoconnect */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_AUTOCONNECT]))
            weechat_log_printf ("  autoconnect. . . . . : null (%s)",
                                (IRC_SERVER_OPTION_BOOLEAN(ptr_server, IRC_SERVER_OPTION_AUTOCONNECT)) ?
                                "on" : "off");
        else
            weechat_log_printf ("  autoconnect. . . . . : %s",
                                (weechat_config_boolean (ptr_server->options[IRC_SERVER_OPTION_AUTOCONNECT])) ?
                                "on" : "off");
        /* autoreconnect */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_AUTORECONNECT]))
            weechat_log_printf ("  autoreconnect. . . . : null (%s)",
                                (IRC_SERVER_OPTION_BOOLEAN(ptr_server, IRC_SERVER_OPTION_AUTORECONNECT)) ?
                                "on" : "off");
        else
            weechat_log_printf ("  autoreconnect. . . . : %s",
                                (weechat_config_boolean (ptr_server->options[IRC_SERVER_OPTION_AUTORECONNECT])) ?
                                "on" : "off");
        /* autoreconnect_delay */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_AUTORECONNECT_DELAY]))
            weechat_log_printf ("  autoreconnect_delay. : null (%d)",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_AUTORECONNECT_DELAY));
        else
            weechat_log_printf ("  autoreconnect_delay. : %d",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_AUTORECONNECT_DELAY]));
        /* nicks */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_NICKS]))
            weechat_log_printf ("  nicks. . . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_NICKS));
        else
            weechat_log_printf ("  nicks. . . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_NICKS]));
        /* nicks_alternate */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_NICKS_ALTERNATE]))
            weechat_log_printf ("  nicks_alternate. . . : null (%s)",
                                (IRC_SERVER_OPTION_BOOLEAN(ptr_server, IRC_SERVER_OPTION_NICKS_ALTERNATE)) ?
                                "on" : "off");
        else
            weechat_log_printf ("  nicks_alternate. . . : %s",
                                (weechat_config_boolean (ptr_server->options[IRC_SERVER_OPTION_NICKS_ALTERNATE])) ?
                                "on" : "off");
        /* username */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_USERNAME]))
            weechat_log_printf ("  username . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_USERNAME));
        else
            weechat_log_printf ("  username . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_USERNAME]));
        /* realname */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_REALNAME]))
            weechat_log_printf ("  realname . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_REALNAME));
        else
            weechat_log_printf ("  realname . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_REALNAME]));
        /* local_hostname */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_LOCAL_HOSTNAME]))
            weechat_log_printf ("  local_hostname . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_LOCAL_HOSTNAME));
        else
            weechat_log_printf ("  local_hostname . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_LOCAL_HOSTNAME]));
        /* usermode */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_USERMODE]))
            weechat_log_printf ("  usermode . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_USERMODE));
        else
            weechat_log_printf ("  usermode . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_USERMODE]));
        /* command */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_COMMAND]))
            weechat_log_printf ("  command. . . . . . . : null");
        else
            weechat_log_printf ("  command. . . . . . . : (hidden)");
        /* command_delay */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_COMMAND_DELAY]))
            weechat_log_printf ("  command_delay. . . . : null (%d)",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_COMMAND_DELAY));
        else
            weechat_log_printf ("  command_delay. . . . : %d",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_COMMAND_DELAY]));
        /* autojoin */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_AUTOJOIN]))
            weechat_log_printf ("  autojoin . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_AUTOJOIN));
        else
            weechat_log_printf ("  autojoin . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_AUTOJOIN]));
        /* autorejoin */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_AUTOREJOIN]))
            weechat_log_printf ("  autorejoin . . . . . : null (%s)",
                                (IRC_SERVER_OPTION_BOOLEAN(ptr_server, IRC_SERVER_OPTION_AUTOREJOIN)) ?
                                "on" : "off");
        else
            weechat_log_printf ("  autorejoin . . . . . : %s",
                                (weechat_config_boolean (ptr_server->options[IRC_SERVER_OPTION_AUTOREJOIN])) ?
                                "on" : "off");
        /* autorejoin_delay */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_AUTOREJOIN_DELAY]))
            weechat_log_printf ("  autorejoin_delay . . : null (%d)",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_AUTOREJOIN_DELAY));
        else
            weechat_log_printf ("  autorejoin_delay . . : %d",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_AUTOREJOIN_DELAY]));
        /* connection_timeout */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_CONNECTION_TIMEOUT]))
            weechat_log_printf ("  connection_timeout . : null (%d)",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_CONNECTION_TIMEOUT));
        else
            weechat_log_printf ("  connection_timeout . : %d",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_CONNECTION_TIMEOUT]));
        /* anti_flood_prio_high */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_HIGH]))
            weechat_log_printf ("  anti_flood_prio_high : null (%d)",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_HIGH));
        else
            weechat_log_printf ("  anti_flood_prio_high : %d",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_HIGH]));
        /* anti_flood_prio_low */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_LOW]))
            weechat_log_printf ("  anti_flood_prio_low. : null (%d)",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_LOW));
        else
            weechat_log_printf ("  anti_flood_prio_low. : %d",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_LOW]));
        /* away_check */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_AWAY_CHECK]))
            weechat_log_printf ("  away_check . . . . . : null (%d)",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_AWAY_CHECK));
        else
            weechat_log_printf ("  away_check . . . . . : %d",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_AWAY_CHECK]));
        /* away_check_max_nicks */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_AWAY_CHECK_MAX_NICKS]))
            weechat_log_printf ("  away_check_max_nicks : null (%d)",
                                IRC_SERVER_OPTION_INTEGER(ptr_server, IRC_SERVER_OPTION_AWAY_CHECK_MAX_NICKS));
        else
            weechat_log_printf ("  away_check_max_nicks : %d",
                                weechat_config_integer (ptr_server->options[IRC_SERVER_OPTION_AWAY_CHECK_MAX_NICKS]));
        /* msg_kick */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_MSG_KICK]))
            weechat_log_printf ("  msg_kick . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_MSG_KICK));
        else
            weechat_log_printf ("  msg_kick . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_MSG_KICK]));
        /* msg_part */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_MSG_PART]))
            weechat_log_printf ("  msg_part . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_MSG_PART));
        else
            weechat_log_printf ("  msg_part . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_MSG_PART]));
        /* msg_quit */
        if (weechat_config_option_is_null (ptr_server->options[IRC_SERVER_OPTION_MSG_QUIT]))
            weechat_log_printf ("  msg_quit . . . . . . : null ('%s')",
                                IRC_SERVER_OPTION_STRING(ptr_server, IRC_SERVER_OPTION_MSG_QUIT));
        else
            weechat_log_printf ("  msg_quit . . . . . . : '%s'",
                                weechat_config_string (ptr_server->options[IRC_SERVER_OPTION_MSG_QUIT]));
        /* other server variables */
        weechat_log_printf ("  temp_server. . . . . : %d",    ptr_server->temp_server);
        weechat_log_printf ("  reloading_from_config: %d",    ptr_server->reloaded_from_config);
        weechat_log_printf ("  reloaded_from_config : %d",    ptr_server->reloaded_from_config);
        weechat_log_printf ("  addresses_eval . . . : '%s'",  ptr_server->addresses_eval);
        weechat_log_printf ("  addresses_count. . . : %d",    ptr_server->addresses_count);
        weechat_log_printf ("  addresses_array. . . : 0x%lx", ptr_server->addresses_array);
        weechat_log_printf ("  ports_array. . . . . : 0x%lx", ptr_server->ports_array);
        weechat_log_printf ("  retry_array. . . . . : 0x%lx", ptr_server->retry_array);
        weechat_log_printf ("  index_current_address: %d",    ptr_server->index_current_address);
        weechat_log_printf ("  current_address. . . : '%s'",  ptr_server->current_address);
        weechat_log_printf ("  current_ip . . . . . : '%s'",  ptr_server->current_ip);
        weechat_log_printf ("  current_port . . . . : %d",    ptr_server->current_port);
        weechat_log_printf ("  current_retry. . . . : %d",    ptr_server->current_retry);
        weechat_log_printf ("  sock . . . . . . . . : %d",    ptr_server->sock);
        weechat_log_printf ("  hook_connect . . . . : 0x%lx", ptr_server->hook_connect);
        weechat_log_printf ("  hook_fd. . . . . . . : 0x%lx", ptr_server->hook_fd);
        weechat_log_printf ("  hook_timer_connection: 0x%lx", ptr_server->hook_timer_connection);
        weechat_log_printf ("  hook_timer_sasl. . . : 0x%lx", ptr_server->hook_timer_sasl);
        weechat_log_printf ("  is_connected . . . . : %d",    ptr_server->is_connected);
        weechat_log_printf ("  ssl_connected. . . . : %d",    ptr_server->ssl_connected);
        weechat_log_printf ("  disconnected . . . . : %d",    ptr_server->disconnected);
#ifdef HAVE_GNUTLS
        weechat_log_printf ("  gnutls_sess. . . . . : 0x%lx", ptr_server->gnutls_sess);
#endif /* HAVE_GNUTLS */
        weechat_log_printf ("  unterminated_message : '%s'",  ptr_server->unterminated_message);
        weechat_log_printf ("  nicks_count. . . . . : %d",    ptr_server->nicks_count);
        weechat_log_printf ("  nicks_array. . . . . : 0x%lx", ptr_server->nicks_array);
        weechat_log_printf ("  nick_first_tried . . : %d",    ptr_server->nick_first_tried);
        weechat_log_printf ("  nick_alternate_number: %d",    ptr_server->nick_alternate_number);
        weechat_log_printf ("  nick . . . . . . . . : '%s'",  ptr_server->nick);
        weechat_log_printf ("  nick_modes . . . . . : '%s'",  ptr_server->nick_modes);
        weechat_log_printf ("  host . . . . . . . . : '%s'",  ptr_server->host);
        weechat_log_printf ("  checking_cap_ls. . . : %d",    ptr_server->checking_cap_ls);
        weechat_log_printf ("  cap_ls . . . . . . . : 0x%lx (hashtable: '%s')",
                            ptr_server->cap_ls,
                            weechat_hashtable_get_string (ptr_server->cap_ls, "keys_values"));
        weechat_log_printf ("  checking_cap_list. . : %d",    ptr_server->checking_cap_list);
        weechat_log_printf ("  cap_list . . . . . . : 0x%lx (hashtable: '%s')",
                            ptr_server->cap_list,
                            weechat_hashtable_get_string (ptr_server->cap_list, "keys_values"));
        weechat_log_printf ("  isupport . . . . . . : '%s'",  ptr_server->isupport);
        weechat_log_printf ("  prefix_modes . . . . : '%s'",  ptr_server->prefix_modes);
        weechat_log_printf ("  prefix_chars . . . . : '%s'",  ptr_server->prefix_chars);
        weechat_log_printf ("  nick_max_length. . . : %d",    ptr_server->nick_max_length);
        weechat_log_printf ("  user_max_length. . . : %d",    ptr_server->user_max_length);
        weechat_log_printf ("  host_max_length. . . : %d",    ptr_server->host_max_length);
        weechat_log_printf ("  casemapping. . . . . : %d (%s)",
                            ptr_server->casemapping,
                            irc_server_casemapping_string[ptr_server->casemapping]);
        weechat_log_printf ("  chantypes. . . . . . : '%s'",  ptr_server->chantypes);
        weechat_log_printf ("  chanmodes. . . . . . : '%s'",  ptr_server->chanmodes);
        weechat_log_printf ("  monitor. . . . . . . : %d",    ptr_server->monitor);
        weechat_log_printf ("  monitor_time . . . . : %lld",  (long long)ptr_server->monitor_time);
        weechat_log_printf ("  reconnect_delay. . . : %d",    ptr_server->reconnect_delay);
        weechat_log_printf ("  reconnect_start. . . : %lld",  (long long)ptr_server->reconnect_start);
        weechat_log_printf ("  command_time . . . . : %lld",  (long long)ptr_server->command_time);
        weechat_log_printf ("  reconnect_join . . . : %d",    ptr_server->reconnect_join);
        weechat_log_printf ("  disable_autojoin . . : %d",    ptr_server->disable_autojoin);
        weechat_log_printf ("  is_away. . . . . . . : %d",    ptr_server->is_away);
        weechat_log_printf ("  away_message . . . . : '%s'",  ptr_server->away_message);
        weechat_log_printf ("  away_time. . . . . . : %lld",  (long long)ptr_server->away_time);
        weechat_log_printf ("  lag. . . . . . . . . : %d",    ptr_server->lag);
        weechat_log_printf ("  lag_displayed. . . . : %d",    ptr_server->lag_displayed);
        weechat_log_printf ("  lag_check_time . . . : tv_sec:%d, tv_usec:%d",
                            ptr_server->lag_check_time.tv_sec,
                            ptr_server->lag_check_time.tv_usec);
        weechat_log_printf ("  lag_next_check . . . : %lld",  (long long)ptr_server->lag_next_check);
        weechat_log_printf ("  lag_last_refresh . . : %lld",  (long long)ptr_server->lag_last_refresh);
        weechat_log_printf ("  cmd_list_regexp. . . : 0x%lx", ptr_server->cmd_list_regexp);
        weechat_log_printf ("  last_user_message. . : %lld",  (long long)ptr_server->last_user_message);
        weechat_log_printf ("  last_away_check. . . : %lld",  (long long)ptr_server->last_away_check);
        weechat_log_printf ("  last_data_purge. . . : %lld",  (long long)ptr_server->last_data_purge);
        for (i = 0; i < IRC_SERVER_NUM_OUTQUEUES_PRIO; i++)
        {
            weechat_log_printf ("  outqueue[%02d] . . . . : 0x%lx", i, ptr_server->outqueue[i]);
            weechat_log_printf ("  last_outqueue[%02d]. . : 0x%lx", i, ptr_server->last_outqueue[i]);
        }
        weechat_log_printf ("  redirects. . . . . . : 0x%lx", ptr_server->redirects);
        weechat_log_printf ("  last_redirect. . . . : 0x%lx", ptr_server->last_redirect);
        weechat_log_printf ("  notify_list. . . . . : 0x%lx", ptr_server->notify_list);
        weechat_log_printf ("  last_notify. . . . . : 0x%lx", ptr_server->last_notify);
        weechat_log_printf ("  notify_count . . . . : %d",    ptr_server->notify_count);
        weechat_log_printf ("  join_manual. . . . . : 0x%lx (hashtable: '%s')",
                            ptr_server->join_manual,
                            weechat_hashtable_get_string (ptr_server->join_manual, "keys_values"));
        weechat_log_printf ("  join_channel_key . . : 0x%lx (hashtable: '%s')",
                            ptr_server->join_channel_key,
                            weechat_hashtable_get_string (ptr_server->join_channel_key, "keys_values"));
        weechat_log_printf ("  join_noswitch. . . . : 0x%lx (hashtable: '%s')",
                            ptr_server->join_noswitch,
                            weechat_hashtable_get_string (ptr_server->join_noswitch, "keys_values"));
        weechat_log_printf ("  buffer . . . . . . . : 0x%lx", ptr_server->buffer);
        weechat_log_printf ("  buffer_as_string . . : 0x%lx", ptr_server->buffer_as_string);
        weechat_log_printf ("  channels . . . . . . : 0x%lx", ptr_server->channels);
        weechat_log_printf ("  last_channel . . . . : 0x%lx", ptr_server->last_channel);
        weechat_log_printf ("  prev_server. . . . . : 0x%lx", ptr_server->prev_server);
        weechat_log_printf ("  next_server. . . . . : 0x%lx", ptr_server->next_server);

        irc_redirect_print_log (ptr_server);

        irc_notify_print_log (ptr_server);

        for (ptr_channel = ptr_server->channels; ptr_channel;
             ptr_channel = ptr_channel->next_channel)
        {
            irc_channel_print_log (ptr_channel);
        }
    }
}