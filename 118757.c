irc_server_get_prefix_char_for_mode (struct t_irc_server *server, char mode)
{
    const char *prefix_chars;
    int index;

    if (server)
    {
        prefix_chars = irc_server_get_prefix_chars (server);
        index = irc_server_get_prefix_mode_index (server, mode);
        if (index >= 0)
            return prefix_chars[index];
    }

    return ' ';
}