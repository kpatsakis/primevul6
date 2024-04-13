irc_server_get_prefix_char_index (struct t_irc_server *server,
                                  char prefix_char)
{
    const char *prefix_chars;
    char *pos;

    if (server)
    {
        prefix_chars = irc_server_get_prefix_chars (server);
        pos = strchr (prefix_chars, prefix_char);
        if (pos)
            return pos - prefix_chars;
    }

    return -1;
}