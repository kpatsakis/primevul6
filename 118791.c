irc_nick_change (struct t_irc_server *server, struct t_irc_channel *channel,
                 struct t_irc_nick *nick, const char *new_nick)
{
    int nick_is_me;

    /* remove nick from nicklist */
    irc_nick_nicklist_remove (server, channel, nick);

    /* update nicks speaking */
    nick_is_me = (irc_server_strcasecmp (server, new_nick, server->nick) == 0) ? 1 : 0;
    if (!nick_is_me)
        irc_channel_nick_speaking_rename (channel, nick->name, new_nick);

    /* change nickname */
    if (nick->name)
        free (nick->name);
    nick->name = strdup (new_nick);
    if (nick->color)
        free (nick->color);
    if (nick_is_me)
        nick->color = strdup (IRC_COLOR_CHAT_NICK_SELF);
    else
        nick->color = irc_nick_find_color (nick->name);

    /* add nick in nicklist */
    irc_nick_nicklist_add (server, channel, nick);
}