irc_server_strcasecmp (struct t_irc_server *server,
                       const char *string1, const char *string2)
{
    int casemapping, rc;

    casemapping = (server) ? server->casemapping : IRC_SERVER_CASEMAPPING_RFC1459;
    switch (casemapping)
    {
        case IRC_SERVER_CASEMAPPING_RFC1459:
            rc = weechat_strcasecmp_range (string1, string2, 30);
            break;
        case IRC_SERVER_CASEMAPPING_STRICT_RFC1459:
            rc = weechat_strcasecmp_range (string1, string2, 29);
            break;
        case IRC_SERVER_CASEMAPPING_ASCII:
            rc = weechat_strcasecmp (string1, string2);
            break;
        default:
            rc = weechat_strcasecmp_range (string1, string2, 30);
            break;
    }
    return rc;
}