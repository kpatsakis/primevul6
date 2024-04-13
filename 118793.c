irc_server_alloc (const char *name)
{
    struct t_irc_server *new_server;
    int i, length;
    char *option_name;

    if (irc_server_casesearch (name))
        return NULL;

    /* alloc memory for new server */
    new_server = malloc (sizeof (*new_server));
    if (!new_server)
    {
        weechat_printf (NULL,
                        _("%s%s: error when allocating new server"),
                        weechat_prefix ("error"), IRC_PLUGIN_NAME);
        return NULL;
    }

    /* add new server to queue */
    new_server->prev_server = last_irc_server;
    new_server->next_server = NULL;
    if (last_irc_server)
        last_irc_server->next_server = new_server;
    else
        irc_servers = new_server;
    last_irc_server = new_server;

    /* set name */
    new_server->name = strdup (name);

    /* internal vars */
    new_server->temp_server = 0;
    new_server->reloading_from_config = 0;
    new_server->reloaded_from_config = 0;
    new_server->addresses_eval = NULL;
    new_server->addresses_count = 0;
    new_server->addresses_array = NULL;
    new_server->ports_array = NULL;
    new_server->retry_array = NULL;
    new_server->index_current_address = 0;
    new_server->current_address = NULL;
    new_server->current_ip = NULL;
    new_server->current_port = 0;
    new_server->current_retry = 0;
    new_server->sock = -1;
    new_server->hook_connect = NULL;
    new_server->hook_fd = NULL;
    new_server->hook_timer_connection = NULL;
    new_server->hook_timer_sasl = NULL;
    new_server->is_connected = 0;
    new_server->ssl_connected = 0;
    new_server->disconnected = 0;
    new_server->unterminated_message = NULL;
    new_server->nicks_count = 0;
    new_server->nicks_array = NULL;
    new_server->nick_first_tried = 0;
    new_server->nick_alternate_number = -1;
    new_server->nick = NULL;
    new_server->nick_modes = NULL;
    new_server->host = NULL;
    new_server->checking_cap_ls = 0;
    new_server->cap_ls = weechat_hashtable_new (32,
                                                WEECHAT_HASHTABLE_STRING,
                                                WEECHAT_HASHTABLE_STRING,
                                                NULL,
                                                NULL);
    new_server->checking_cap_list = 0;
    new_server->cap_list = weechat_hashtable_new (32,
                                                  WEECHAT_HASHTABLE_STRING,
                                                  WEECHAT_HASHTABLE_STRING,
                                                  NULL,
                                                  NULL);
    new_server->isupport = NULL;
    new_server->prefix_modes = NULL;
    new_server->prefix_chars = NULL;
    new_server->nick_max_length = 0;
    new_server->user_max_length = 0;
    new_server->host_max_length = 0;
    new_server->casemapping = IRC_SERVER_CASEMAPPING_RFC1459;
    new_server->chantypes = NULL;
    new_server->chanmodes = NULL;
    new_server->monitor = 0;
    new_server->monitor_time = 0;
    new_server->reconnect_delay = 0;
    new_server->reconnect_start = 0;
    new_server->command_time = 0;
    new_server->reconnect_join = 0;
    new_server->disable_autojoin = 0;
    new_server->is_away = 0;
    new_server->away_message = NULL;
    new_server->away_time = 0;
    new_server->lag = 0;
    new_server->lag_displayed = -1;
    new_server->lag_check_time.tv_sec = 0;
    new_server->lag_check_time.tv_usec = 0;
    new_server->lag_next_check = time (NULL) +
        weechat_config_integer (irc_config_network_lag_check);
    new_server->lag_last_refresh = 0;
    new_server->cmd_list_regexp = NULL;
    new_server->last_user_message = 0;
    new_server->last_away_check = 0;
    new_server->last_data_purge = 0;
    for (i = 0; i < IRC_SERVER_NUM_OUTQUEUES_PRIO; i++)
    {
        new_server->outqueue[i] = NULL;
        new_server->last_outqueue[i] = NULL;
    }
    new_server->redirects = NULL;
    new_server->last_redirect = NULL;
    new_server->notify_list = NULL;
    new_server->last_notify = NULL;
    new_server->notify_count = 0;
    new_server->join_manual = weechat_hashtable_new (
        32,
        WEECHAT_HASHTABLE_STRING,
        WEECHAT_HASHTABLE_TIME,
        NULL, NULL);
    new_server->join_channel_key = weechat_hashtable_new (
        32,
        WEECHAT_HASHTABLE_STRING,
        WEECHAT_HASHTABLE_STRING,
        NULL, NULL);
    new_server->join_noswitch = weechat_hashtable_new (
        32,
        WEECHAT_HASHTABLE_STRING,
        WEECHAT_HASHTABLE_TIME,
        NULL, NULL);
    new_server->buffer = NULL;
    new_server->buffer_as_string = NULL;
    new_server->channels = NULL;
    new_server->last_channel = NULL;

    /* create options with null value */
    for (i = 0; i < IRC_SERVER_NUM_OPTIONS; i++)
    {
        length = strlen (new_server->name) + 1 +
            strlen (irc_server_options[i][0]) +
            512 +  /* inherited option name (irc.server_default.xxx) */
            1;
        option_name = malloc (length);
        if (option_name)
        {
            snprintf (option_name, length, "%s.%s << irc.server_default.%s",
                      new_server->name,
                      irc_server_options[i][0],
                      irc_server_options[i][0]);
            new_server->options[i] = irc_config_server_new_option (
                irc_config_file,
                irc_config_section_server,
                i,
                option_name,
                NULL,
                NULL,
                1,
                &irc_config_server_check_value_cb,
                irc_server_options[i][0],
                NULL,
                &irc_config_server_change_cb,
                irc_server_options[i][0],
                NULL);
            irc_config_server_change_cb (irc_server_options[i][0], NULL,
                                         new_server->options[i]);
            free (option_name);
        }
    }

    return new_server;
}