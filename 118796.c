irc_nick_mode_for_display (struct t_irc_server *server, struct t_irc_nick *nick,
                           int prefix)
{
    static char result[32];
    char str_prefix[2];
    int nick_mode;
    const char *str_prefix_color;

    str_prefix[0] = (nick) ? nick->prefix[0] : '\0';
    str_prefix[1] = '\0';

    nick_mode = weechat_config_integer (irc_config_look_nick_mode);
    if ((nick_mode == IRC_CONFIG_LOOK_NICK_MODE_BOTH)
        || (prefix && (nick_mode == IRC_CONFIG_LOOK_NICK_MODE_PREFIX))
        || (!prefix && (nick_mode == IRC_CONFIG_LOOK_NICK_MODE_ACTION)))
    {
        if (nick)
        {
            if ((str_prefix[0] == ' ')
                && (!prefix || !weechat_config_boolean (irc_config_look_nick_mode_empty)))
            {
                str_prefix[0] = '\0';
            }
            str_prefix_color = weechat_color (
                irc_nick_get_prefix_color_name (server, nick->prefix[0]));
        }
        else
        {
            str_prefix[0] = (prefix
                             && weechat_config_boolean (irc_config_look_nick_mode_empty)) ?
                ' ' : '\0';
            str_prefix_color = IRC_COLOR_RESET;
        }
    }
    else
    {
        str_prefix[0] = '\0';
        str_prefix_color = IRC_COLOR_RESET;
    }

    snprintf (result, sizeof (result), "%s%s", str_prefix_color, str_prefix);

    return result;
}