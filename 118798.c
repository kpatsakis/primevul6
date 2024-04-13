irc_server_get_default_msg (const char *default_msg,
                            struct t_irc_server *server,
                            const char *channel_name)
{
    char *version;
    struct t_hashtable *extra_vars;
    char *msg, *res;

    /*
     * "%v" for version is deprecated since WeeChat 1.6, where
     * an expression ${info:version} is preferred, so we replace
     * the "%v" with version only if there's no "${...}" in string
     */
    if (strstr (default_msg, "%v") && !strstr (default_msg, "${"))
    {
        version = weechat_info_get ("version", "");
        res = weechat_string_replace (default_msg, "%v",
                                      (version) ? version : "");
        if (version)
            free (version);
        return res;
    }

    extra_vars = weechat_hashtable_new (32,
                                        WEECHAT_HASHTABLE_STRING,
                                        WEECHAT_HASHTABLE_STRING,
                                        NULL,
                                        NULL);
    if (extra_vars)
    {
        weechat_hashtable_set (extra_vars, "server", server->name);
        weechat_hashtable_set (extra_vars, "channel",
                               (channel_name) ? channel_name : "");
        weechat_hashtable_set (extra_vars, "nick", server->nick);
    }

    msg = weechat_string_eval_expression (default_msg, NULL, extra_vars, NULL);

    if (extra_vars)
        weechat_hashtable_free (extra_vars);

    return msg;
}