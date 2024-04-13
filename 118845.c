irc_server_check_certificate_fingerprint (struct t_irc_server *server,
                                          gnutls_x509_crt_t certificate,
                                          const char *good_fingerprints)
{
    unsigned char *fingerprint_server[IRC_FINGERPRINT_NUM_ALGOS];
    char **fingerprints;
    int i, rc, algo;
    size_t size_bits, size_bytes;

    for (i = 0; i < IRC_FINGERPRINT_NUM_ALGOS; i++)
    {
        fingerprint_server[i] = NULL;
    }

    /* split good_fingerprints */
    fingerprints = weechat_string_split (good_fingerprints, ",", NULL,
                                         WEECHAT_STRING_SPLIT_STRIP_LEFT
                                         | WEECHAT_STRING_SPLIT_STRIP_RIGHT
                                         | WEECHAT_STRING_SPLIT_COLLAPSE_SEPS,
                                         0, NULL);
    if (!fingerprints)
        return 0;

    rc = 0;

    for (i = 0; fingerprints[i]; i++)
    {
        size_bits = strlen (fingerprints[i]) * 4;
        size_bytes = size_bits / 8;

        algo = irc_server_fingerprint_search_algo_with_size (size_bits);
        if (algo < 0)
            continue;

        if (!fingerprint_server[algo])
        {
            fingerprint_server[algo] = malloc (size_bytes);
            if (fingerprint_server[algo])
            {
                /* calculate the fingerprint for the certificate */
                if (gnutls_x509_crt_get_fingerprint (
                        certificate,
                        irc_fingerprint_digest_algos[algo],
                        fingerprint_server[algo],
                        &size_bytes) != GNUTLS_E_SUCCESS)
                {
                    weechat_printf (
                        server->buffer,
                        _("%sgnutls: failed to calculate certificate "
                          "fingerprint (%s)"),
                        weechat_prefix ("error"),
                        irc_fingerprint_digest_algos_name[algo]);
                    free (fingerprint_server[algo]);
                    fingerprint_server[algo] = NULL;
                }
            }
            else
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: not enough memory (%s)"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME,
                    "fingerprint");
            }
        }

        if (fingerprint_server[algo])
        {
            /* check if the fingerprint matches */
            if (irc_server_compare_fingerprints (fingerprints[i],
                                                 fingerprint_server[algo],
                                                 size_bytes) == 0)
            {
                rc = 1;
                break;
            }
        }
    }

    weechat_string_free_split (fingerprints);

    for (i = 0; i < IRC_FINGERPRINT_NUM_ALGOS; i++)
    {
        if (fingerprint_server[i])
            free (fingerprint_server[i]);
    }

    return rc;
}