irc_nick_set_prefix (struct t_irc_server *server, struct t_irc_nick *nick,
                     int set, char prefix)
{
    int index;

    if (!nick)
        return;

    index = irc_server_get_prefix_char_index (server, prefix);
    if (index >= 0)
    {
        nick->prefixes[index] = (set) ? prefix : ' ';
        irc_nick_set_current_prefix (nick);
    }
}