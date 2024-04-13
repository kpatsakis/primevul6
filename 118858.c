irc_server_eval_expression (struct t_irc_server *server, const char *string)
{
    struct t_hashtable *pointers, *extra_vars;
    char *value;

    pointers = weechat_hashtable_new (
        32,
        WEECHAT_HASHTABLE_STRING,
        WEECHAT_HASHTABLE_POINTER,
        NULL, NULL);
    extra_vars = weechat_hashtable_new (
        32,
        WEECHAT_HASHTABLE_STRING,
        WEECHAT_HASHTABLE_STRING,
        NULL, NULL);

    if (server)
    {
        if (pointers)
            weechat_hashtable_set (pointers, "irc_server", server);
        if (extra_vars)
            weechat_hashtable_set (extra_vars, "server", server->name);
    }

    value = weechat_string_eval_expression (string,
                                            pointers, extra_vars, NULL);

    if (pointers)
        weechat_hashtable_free (pointers);
    if (extra_vars)
        weechat_hashtable_free (extra_vars);

    return value;
}