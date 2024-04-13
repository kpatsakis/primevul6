irc_nick_set_away (struct t_irc_server *server, struct t_irc_channel *channel,
                   struct t_irc_nick *nick, int is_away)
{
    char *color;

    if (is_away != nick->away)
    {
        nick->away = is_away;
        color = irc_nick_get_color_for_nicklist (server, nick);
        irc_nick_nicklist_set (channel, nick, "color", color);
        if (color)
            free (color);
    }
}