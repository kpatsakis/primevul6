irc_server_set_nicks (struct t_irc_server *server, const char *nicks)
{
    char *nicks2;

    /* free data */
    server->nicks_count = 0;
    if (server->nicks_array)
    {
        weechat_string_free_split (server->nicks_array);
        server->nicks_array = NULL;
    }

    /* evaluate value */
    nicks2 = irc_server_eval_expression (server, nicks);

    /* set new nicks */
    server->nicks_array = weechat_string_split (
        (nicks2) ? nicks2 : IRC_SERVER_DEFAULT_NICKS,
        ",",
        NULL,
        WEECHAT_STRING_SPLIT_STRIP_LEFT
        | WEECHAT_STRING_SPLIT_STRIP_RIGHT
        | WEECHAT_STRING_SPLIT_COLLAPSE_SEPS,
        0,
        &server->nicks_count);

    if (nicks2)
        free (nicks2);
}