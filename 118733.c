irc_nick_as_prefix (struct t_irc_server *server, struct t_irc_nick *nick,
                    const char *nickname, const char *force_color)
{
    static char result[256];
    char *color;

    if (force_color)
        color = strdup (force_color);
    else if (nick)
        color = strdup (nick->color);
    else if (nickname)
        color = irc_nick_find_color (nickname);
    else
        color = strdup (IRC_COLOR_CHAT_NICK);

    snprintf (result, sizeof (result), "%s%s%s\t",
              irc_nick_mode_for_display (server, nick, 1),
              color,
              (nick) ? nick->name : nickname);

    if (color)
        free (color);

    return result;
}