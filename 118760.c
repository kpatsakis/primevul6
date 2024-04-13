irc_nick_color_for_msg (struct t_irc_server *server, int server_message,
                        struct t_irc_nick *nick, const char *nickname)
{
    static char color[16][64];
    static int index_color = 0;
    char *color_found;

    if (server_message
        && !weechat_config_boolean (irc_config_look_color_nicks_in_server_messages))
    {
        return IRC_COLOR_CHAT_NICK;
    }

    if (nick)
        return nick->color;

    if (nickname)
    {
        if (server
            && (irc_server_strcasecmp (server, nickname, server->nick) == 0))
        {
            return IRC_COLOR_CHAT_NICK_SELF;
        }
        color_found = irc_nick_find_color (nickname);
        index_color = (index_color + 1) % 16;
        snprintf (color[index_color], sizeof (color[index_color]),
                  "%s",
                  color_found);
        if (color_found)
            free (color_found);
        return color[index_color];
    }

    return IRC_COLOR_CHAT_NICK;
}