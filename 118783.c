irc_nick_get_nicklist_group (struct t_irc_server *server,
                             struct t_gui_buffer *buffer,
                             struct t_irc_nick *nick)
{
    int index;
    char str_group[2];
    const char *prefix_modes;
    struct t_gui_nick_group *ptr_group;

    if (!server || !buffer || !nick)
        return NULL;

    ptr_group = NULL;
    index = irc_server_get_prefix_char_index (server, nick->prefix[0]);
    if (index < 0)
    {
        ptr_group = weechat_nicklist_search_group (buffer, NULL,
                                                   IRC_NICK_GROUP_OTHER_NAME);
    }
    else
    {
        prefix_modes = irc_server_get_prefix_modes (server);
        str_group[0] = prefix_modes[index];
        str_group[1] = '\0';
        ptr_group = weechat_nicklist_search_group (buffer, NULL, str_group);
    }

    return ptr_group;
}