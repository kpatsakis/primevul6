irc_nick_nicklist_set (struct t_irc_channel *channel,
                       struct t_irc_nick *nick,
                       const char *property, const char *value)
{
    struct t_gui_nick *ptr_nick;

    ptr_nick = weechat_nicklist_search_nick (channel->buffer, NULL, nick->name);
    if (ptr_nick)
    {
        weechat_nicklist_nick_set (channel->buffer, ptr_nick, property, value);
    }
}