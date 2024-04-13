irc_server_strncasecmp (struct t_irc_server *server,
                        const char *string1, const char *string2, int max)
{
    int casemapping, rc;

    casemapping = (server) ? server->casemapping : IRC_SERVER_CASEMAPPING_RFC1459;
    switch (casemapping)
    {
        case IRC_SERVER_CASEMAPPING_RFC1459:
            rc = weechat_strncasecmp_range (string1, string2, max, 30);
            break;
        case IRC_SERVER_CASEMAPPING_STRICT_RFC1459:
            rc = weechat_strncasecmp_range (string1, string2, max, 29);
            break;
        case IRC_SERVER_CASEMAPPING_ASCII:
            rc = weechat_strncasecmp (string1, string2, max);
            break;
        default:
            rc = weechat_strncasecmp_range (string1, string2, max, 30);
            break;
    }
    return rc;
}