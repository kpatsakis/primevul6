irc_nick_get_color_for_nicklist (struct t_irc_server *server,
                                 struct t_irc_nick *nick)
{
    static char *nick_color_bar_fg = "bar_fg";
    static char *nick_color_self = "weechat.color.chat_nick_self";
    static char *nick_color_away = "weechat.color.nicklist_away";

    if (nick->away)
        return strdup (nick_color_away);

    if (weechat_config_boolean (irc_config_look_color_nicks_in_nicklist))
    {
        if (irc_server_strcasecmp (server, nick->name, server->nick) == 0)
            return strdup (nick_color_self);
        else
            return irc_nick_find_color_name (nick->name);
    }

    return strdup (nick_color_bar_fg);
}