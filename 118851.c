irc_server_prefix_char_statusmsg (struct t_irc_server *server,
                                  char prefix_char)
{
    const char *support_statusmsg;

    support_statusmsg = irc_server_get_isupport_value (server, "STATUSMSG");
    if (support_statusmsg)
        return (strchr (support_statusmsg, prefix_char)) ? 1 : 0;

    return (irc_server_get_prefix_char_index (server, prefix_char) >= 0) ?
        1 : 0;
}