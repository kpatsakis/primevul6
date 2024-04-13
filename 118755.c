irc_server_eval_fingerprint (struct t_irc_server *server)
{
#ifdef HAVE_GNUTLS
    const char *ptr_fingerprint;
    char *fingerprint_eval, **fingerprints, *str_sizes;
    int i, j, rc, algo, length;

    ptr_fingerprint = IRC_SERVER_OPTION_STRING(server,
                                               IRC_SERVER_OPTION_SSL_FINGERPRINT);

    /* empty fingerprint is just ignored (considered OK) */
    if (!ptr_fingerprint || !ptr_fingerprint[0])
        return strdup ("");

    /* evaluate fingerprint */
    fingerprint_eval = irc_server_eval_expression (server, ptr_fingerprint);
    if (!fingerprint_eval || !fingerprint_eval[0])
    {
        weechat_printf (
            server->buffer,
            _("%s%s: the evaluated fingerprint for server \"%s\" must not be "
              "empty"),
            weechat_prefix ("error"),
            IRC_PLUGIN_NAME,
            server->name);
        if (fingerprint_eval)
            free (fingerprint_eval);
        return NULL;
    }

    /* split fingerprint */
    fingerprints = weechat_string_split (fingerprint_eval, ",", NULL,
                                         WEECHAT_STRING_SPLIT_STRIP_LEFT
                                         | WEECHAT_STRING_SPLIT_STRIP_RIGHT
                                         | WEECHAT_STRING_SPLIT_COLLAPSE_SEPS,
                                         0, NULL);
    if (!fingerprints)
        return fingerprint_eval;

    rc = 0;
    for (i = 0; fingerprints[i]; i++)
    {
        length = strlen (fingerprints[i]);
        algo = irc_server_fingerprint_search_algo_with_size (length * 4);
        if (algo < 0)
        {
            rc = -1;
            break;
        }
        for (j = 0; j < length; j++)
        {
            if (!isxdigit ((unsigned char)fingerprints[i][j]))
            {
                rc = -2;
                break;
            }
        }
        if (rc < 0)
            break;
    }
    weechat_string_free_split (fingerprints);
    switch (rc)
    {
        case -1:  /* invalid size */
            str_sizes = irc_server_fingerprint_str_sizes ();
            weechat_printf (
                server->buffer,
                _("%s%s: invalid fingerprint size for server \"%s\", the "
                  "number of hexadecimal digits must be "
                  "one of: %s"),
                weechat_prefix ("error"),
                IRC_PLUGIN_NAME,
                server->name,
                (str_sizes) ? str_sizes : "?");
            if (str_sizes)
                free (str_sizes);
            free (fingerprint_eval);
            return NULL;
        case -2:  /* invalid content */
            weechat_printf (
                server->buffer,
                _("%s%s: invalid fingerprint for server \"%s\", it must "
                  "contain only hexadecimal digits (0-9, "
                  "a-f)"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, server->name);
            free (fingerprint_eval);
            return NULL;
    }
    return fingerprint_eval;
#else
    /* make C compiler happy */
    (void) server;

    return strdup ("");
#endif /* HAVE_GNUTLS */
}