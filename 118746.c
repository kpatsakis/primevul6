irc_nick_get_prefix_color_name (struct t_irc_server *server, char prefix)
{
    static char *default_color = "";
    const char *prefix_modes, *color;
    char mode[2];
    int index;

    if (irc_config_hashtable_nick_prefixes)
    {
        mode[0] = ' ';
        mode[1] = '\0';

        index = irc_server_get_prefix_char_index (server, prefix);
        if (index >= 0)
        {
            prefix_modes = irc_server_get_prefix_modes (server);
            mode[0] = prefix_modes[index];
            color = weechat_hashtable_get (irc_config_hashtable_nick_prefixes,
                                           mode);
            if (color)
                return color;
        }

        /* fallback to "*" if no color is found with mode */
        mode[0] = '*';
        color = weechat_hashtable_get (irc_config_hashtable_nick_prefixes,
                                       mode);
        if (color)
            return color;
    }

    /* no color by default */
    return default_color;
}