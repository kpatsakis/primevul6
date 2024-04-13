irc_nick_nicklist_remove (struct t_irc_server *server,
                          struct t_irc_channel *channel,
                          struct t_irc_nick *nick)
{
    struct t_gui_nick_group *ptr_group;

    ptr_group = irc_nick_get_nicklist_group (server, channel->buffer, nick);
    weechat_nicklist_remove_nick (channel->buffer,
                                  weechat_nicklist_search_nick (channel->buffer,
                                                                ptr_group,
                                                                nick->name));
}