irc_server_get_prefix_mode_for_char (struct t_irc_server *server,
                                     char prefix_char)
{
    const char *prefix_modes;
    int index;

    if (server)
    {
        prefix_modes = irc_server_get_prefix_modes (server);
        index = irc_server_get_prefix_char_index (server, prefix_char);
        if (index >= 0)
            return prefix_modes[index];
    }

    return ' ';
}