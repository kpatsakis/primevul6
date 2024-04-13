irc_server_get_prefix_mode_index (struct t_irc_server *server, char mode)
{
    const char *prefix_modes;
    char *pos;

    if (server)
    {
        prefix_modes = irc_server_get_prefix_modes (server);
        pos = strchr (prefix_modes, mode);
        if (pos)
            return pos - prefix_modes;
    }

    return -1;
}