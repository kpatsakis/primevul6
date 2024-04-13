irc_server_create_buffer (struct t_irc_server *server)
{
    char buffer_name[256], charset_modifier[256];
    struct t_gui_buffer *ptr_buffer_for_merge;

    ptr_buffer_for_merge = NULL;
    switch (weechat_config_integer (irc_config_look_server_buffer))
    {
        case IRC_CONFIG_LOOK_SERVER_BUFFER_MERGE_WITH_CORE:
            /* merge with WeeChat core buffer */
            ptr_buffer_for_merge = weechat_buffer_search_main ();
            break;
        case IRC_CONFIG_LOOK_SERVER_BUFFER_MERGE_WITHOUT_CORE:
            /* find buffer used to merge all IRC server buffers */
            ptr_buffer_for_merge = irc_buffer_search_server_lowest_number ();
            break;
    }

    snprintf (buffer_name, sizeof (buffer_name),
              "server.%s", server->name);
    server->buffer = weechat_buffer_new (buffer_name,
                                         &irc_input_data_cb, NULL, NULL,
                                         &irc_buffer_close_cb, NULL, NULL);
    if (!server->buffer)
        return NULL;

    if (!weechat_buffer_get_integer (server->buffer, "short_name_is_set"))
        weechat_buffer_set (server->buffer, "short_name", server->name);
    weechat_buffer_set (server->buffer, "localvar_set_type", "server");
    weechat_buffer_set (server->buffer, "localvar_set_server", server->name);
    weechat_buffer_set (server->buffer, "localvar_set_channel", server->name);
    snprintf (charset_modifier, sizeof (charset_modifier),
              "irc.%s", server->name);
    weechat_buffer_set (server->buffer, "localvar_set_charset_modifier",
                        charset_modifier);

    (void) weechat_hook_signal_send ("logger_backlog",
                                     WEECHAT_HOOK_SIGNAL_POINTER,
                                     server->buffer);

    if (weechat_config_boolean (irc_config_network_send_unknown_commands))
        weechat_buffer_set (server->buffer, "input_get_unknown_commands", "1");

    /* set highlights settings on server buffer */
    weechat_buffer_set (server->buffer, "highlight_words_add",
                        weechat_config_string (irc_config_look_highlight_server));
    if (weechat_config_string (irc_config_look_highlight_tags_restrict)
        && weechat_config_string (irc_config_look_highlight_tags_restrict)[0])
    {
        weechat_buffer_set (
            server->buffer, "highlight_tags_restrict",
            weechat_config_string (irc_config_look_highlight_tags_restrict));
    }

    irc_server_set_buffer_title (server);

    /*
     * merge buffer if needed: if merge with(out) core set, and if no layout
     * number is assigned for this buffer (if layout number is assigned, then
     * buffer was already moved/merged by WeeChat core)
     */
    if (ptr_buffer_for_merge
        && (weechat_buffer_get_integer (server->buffer, "layout_number") < 1))
    {
        weechat_buffer_merge (server->buffer, ptr_buffer_for_merge);
    }

    (void) weechat_hook_signal_send ("irc_server_opened",
                                     WEECHAT_HOOK_SIGNAL_POINTER,
                                     server->buffer);

    return server->buffer;
}