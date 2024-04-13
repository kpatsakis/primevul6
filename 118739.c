irc_nick_is_op (struct t_irc_server *server, struct t_irc_nick *nick)
{
    int index;

    if (nick->prefix[0] == ' ')
        return 0;

    index = irc_server_get_prefix_char_index (server, nick->prefix[0]);
    if (index < 0)
        return 0;

    return (index <= irc_server_get_prefix_mode_index (server, 'o')) ? 1 : 0;
}