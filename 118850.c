irc_server_search_option (const char *option_name)
{
    int i;

    if (!option_name)
        return -1;

    for (i = 0; i < IRC_SERVER_NUM_OPTIONS; i++)
    {
        if (weechat_strcasecmp (irc_server_options[i][0], option_name) == 0)
            return i;
    }

    /* server option not found */
    return -1;
}