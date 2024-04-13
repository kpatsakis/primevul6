irc_nick_nicklist_add (struct t_irc_server *server,
                       struct t_irc_channel *channel,
                       struct t_irc_nick *nick)
{
    struct t_gui_nick_group *ptr_group;
    char *color;

    ptr_group = irc_nick_get_nicklist_group (server, channel->buffer, nick);
    color = irc_nick_get_color_for_nicklist (server, nick);
    weechat_nicklist_add_nick (channel->buffer, ptr_group,
                               nick->name,
                               color,
                               nick->prefix,
                               irc_nick_get_prefix_color_name (server, nick->prefix[0]),
                               1);
    if (color)
        free (color);
}