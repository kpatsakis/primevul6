irc_nick_has_prefix_mode (struct t_irc_server *server, struct t_irc_nick *nick,
                          char prefix_mode)
{
    char prefix_char;

    prefix_char = irc_server_get_prefix_char_for_mode (server, prefix_mode);
    if (prefix_char == ' ')
        return 0;

    return (strchr (nick->prefixes, prefix_char)) ? 1 : 0;
}