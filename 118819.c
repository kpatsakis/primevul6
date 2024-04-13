irc_server_search_casemapping (const char *casemapping)
{
    int i;

    for (i = 0; i < IRC_SERVER_NUM_CASEMAPPING; i++)
    {
        if (weechat_strcasecmp (irc_server_casemapping_string[i], casemapping) == 0)
            return i;
    }

    /* casemapping not found */
    return -1;
}