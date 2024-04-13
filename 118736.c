irc_server_gnutls_callback (const void *pointer, void *data,
                            gnutls_session_t tls_session,
                            const gnutls_datum_t *req_ca, int nreq,
                            const gnutls_pk_algorithm_t *pk_algos,
                            int pk_algos_len,
#if LIBGNUTLS_VERSION_NUMBER >= 0x020b00 /* 2.11.0 */
                            gnutls_retr2_st *answer,
#else
                            gnutls_retr_st *answer,
#endif /* LIBGNUTLS_VERSION_NUMBER >= 0x020b00 */
                            int action)
{
    struct t_irc_server *server;
#if LIBGNUTLS_VERSION_NUMBER >= 0x020b00 /* 2.11.0 */
    gnutls_retr2_st tls_struct;
#else
    gnutls_retr_st tls_struct;
#endif /* LIBGNUTLS_VERSION_NUMBER >= 0x020b00 */
    gnutls_x509_crt_t cert_temp;
    const gnutls_datum_t *cert_list;
    gnutls_datum_t filedatum;
    unsigned int i, cert_list_len, status;
    time_t cert_time;
    char *cert_path0, *cert_path1, *cert_path2, *cert_str, *fingerprint_eval;
    char *weechat_dir, *ssl_password;
    const char *ptr_fingerprint;
    int rc, ret, fingerprint_match, hostname_match, cert_temp_init;
#if LIBGNUTLS_VERSION_NUMBER >= 0x010706 /* 1.7.6 */
    gnutls_datum_t cinfo;
    int rinfo;
#endif /* LIBGNUTLS_VERSION_NUMBER >= 0x010706 */

    /* make C compiler happy */
    (void) data;
    (void) req_ca;
    (void) nreq;
    (void) pk_algos;
    (void) pk_algos_len;

    rc = 0;

    if (!pointer)
        return -1;

    server = (struct t_irc_server *) pointer;
    cert_temp_init = 0;
    cert_list = NULL;
    cert_list_len = 0;
    fingerprint_eval = NULL;
    weechat_dir = NULL;

    if (action == WEECHAT_HOOK_CONNECT_GNUTLS_CB_VERIFY_CERT)
    {
        weechat_printf (
            server->buffer,
            _("%sgnutls: connected using %d-bit Diffie-Hellman shared secret "
              "exchange"),
            weechat_prefix ("network"),
            IRC_SERVER_OPTION_INTEGER (server,
                                       IRC_SERVER_OPTION_SSL_DHKEY_SIZE));

        /* initialize the certificate structure */
        if (gnutls_x509_crt_init (&cert_temp) != GNUTLS_E_SUCCESS)
        {
            weechat_printf (
                server->buffer,
                _("%sgnutls: failed to initialize certificate structure"),
                weechat_prefix ("error"));
            rc = -1;
            goto end;
        }

        /* flag to do the "deinit" (at the end of function) */
        cert_temp_init = 1;

        /* get fingerprint option in server */
        ptr_fingerprint = IRC_SERVER_OPTION_STRING(server,
                                                   IRC_SERVER_OPTION_SSL_FINGERPRINT);
        fingerprint_eval = irc_server_eval_fingerprint (server);
        if (!fingerprint_eval)
        {
            rc = -1;
            goto end;
        }

        /* set match options */
        fingerprint_match = (ptr_fingerprint && ptr_fingerprint[0]) ? 0 : 1;
        hostname_match = 0;

        /* get the peer's raw certificate (chain) as sent by the peer */
        cert_list = gnutls_certificate_get_peers (tls_session, &cert_list_len);
        if (cert_list)
        {
            weechat_printf (
                server->buffer,
                NG_("%sgnutls: receiving %d certificate",
                    "%sgnutls: receiving %d certificates",
                    cert_list_len),
                weechat_prefix ("network"),
                cert_list_len);

            for (i = 0; i < cert_list_len; i++)
            {
                if (gnutls_x509_crt_import (cert_temp,
                                            &cert_list[i],
                                            GNUTLS_X509_FMT_DER) != GNUTLS_E_SUCCESS)
                {
                    weechat_printf (
                        server->buffer,
                        _("%sgnutls: failed to import certificate[%d]"),
                        weechat_prefix ("error"), i + 1);
                    rc = -1;
                    goto end;
                }

                /* checks on first certificate received */
                if (i == 0)
                {
                    /* check if fingerprint matches the first certificate */
                    if (fingerprint_eval && fingerprint_eval[0])
                    {
                        fingerprint_match = irc_server_check_certificate_fingerprint (
                            server, cert_temp, fingerprint_eval);
                    }
                    /* check if hostname matches in the first certificate */
                    if (gnutls_x509_crt_check_hostname (cert_temp,
                                                        server->current_address) != 0)
                    {
                        hostname_match = 1;
                    }
                }
#if LIBGNUTLS_VERSION_NUMBER >= 0x010706 /* 1.7.6 */
                /* display infos about certificate */
#if LIBGNUTLS_VERSION_NUMBER < 0x020400 /* 2.4.0 */
                rinfo = gnutls_x509_crt_print (cert_temp,
                                               GNUTLS_X509_CRT_ONELINE, &cinfo);
#else
                rinfo = gnutls_x509_crt_print (cert_temp,
                                               GNUTLS_CRT_PRINT_ONELINE, &cinfo);
#endif /*  LIBGNUTLS_VERSION_NUMBER < 0x020400 */
                if (rinfo == 0)
                {
                    weechat_printf (
                        server->buffer,
                        _("%s - certificate[%d] info:"),
                        weechat_prefix ("network"), i + 1);
                    weechat_printf (
                        server->buffer,
                        "%s   - %s",
                        weechat_prefix ("network"), cinfo.data);
                    gnutls_free (cinfo.data);
                }
#endif /* LIBGNUTLS_VERSION_NUMBER >= 0x010706 */
                /* check dates, only if fingerprint is not set */
                if (!ptr_fingerprint || !ptr_fingerprint[0])
                {
                    /* check expiration date */
                    cert_time = gnutls_x509_crt_get_expiration_time (cert_temp);
                    if (cert_time < time (NULL))
                    {
                        weechat_printf (
                            server->buffer,
                            _("%sgnutls: certificate has expired"),
                            weechat_prefix ("error"));
                        rc = -1;
                    }
                    /* check activation date */
                    cert_time = gnutls_x509_crt_get_activation_time (cert_temp);
                    if (cert_time > time (NULL))
                    {
                        weechat_printf (
                            server->buffer,
                            _("%sgnutls: certificate is not yet activated"),
                            weechat_prefix ("error"));
                        rc = -1;
                    }
                }
            }

            /*
             * if fingerprint is set, display if matches, and don't check
             * anything else
             */
            if (ptr_fingerprint && ptr_fingerprint[0])
            {
                if (fingerprint_match)
                {
                    weechat_printf (
                        server->buffer,
                        _("%sgnutls: certificate fingerprint matches"),
                        weechat_prefix ("network"));
                }
                else
                {
                    weechat_printf (
                        server->buffer,
                        _("%sgnutls: certificate fingerprint does NOT match "
                          "(check value of option "
                          "irc.server.%s.ssl_fingerprint)"),
                        weechat_prefix ("error"), server->name);
                    rc = -1;
                }
                goto end;
            }

            if (!hostname_match)
            {
                weechat_printf (
                    server->buffer,
                    _("%sgnutls: the hostname in the certificate does NOT "
                      "match \"%s\""),
                    weechat_prefix ("error"), server->current_address);
                rc = -1;
            }
        }

        /* verify the peer’s certificate */
        if (gnutls_certificate_verify_peers2 (tls_session, &status) < 0)
        {
            weechat_printf (
                server->buffer,
                _("%sgnutls: error while checking peer's certificate"),
                weechat_prefix ("error"));
            rc = -1;
            goto end;
        }

        /* check if certificate is trusted */
        if (status & GNUTLS_CERT_INVALID)
        {
            weechat_printf (
                server->buffer,
                _("%sgnutls: peer's certificate is NOT trusted"),
                weechat_prefix ("error"));
            rc = -1;
        }
        else
        {
            weechat_printf (
                server->buffer,
                _("%sgnutls: peer's certificate is trusted"),
                weechat_prefix ("network"));
        }

        /* check if certificate issuer is known */
        if (status & GNUTLS_CERT_SIGNER_NOT_FOUND)
        {
            weechat_printf (
                server->buffer,
                _("%sgnutls: peer's certificate issuer is unknown"),
                weechat_prefix ("error"));
            rc = -1;
        }

        /* check that certificate is not revoked */
        if (status & GNUTLS_CERT_REVOKED)
        {
            weechat_printf (
                server->buffer,
                _("%sgnutls: the certificate has been revoked"),
                weechat_prefix ("error"));
            rc = -1;
        }
    }
    else if (action == WEECHAT_HOOK_CONNECT_GNUTLS_CB_SET_CERT)
    {
        /* using client certificate if it exists */
        cert_path0 = (char *) IRC_SERVER_OPTION_STRING(
            server, IRC_SERVER_OPTION_SSL_CERT);
        if (cert_path0 && cert_path0[0])
        {
            weechat_dir = weechat_info_get ("weechat_dir", "");
            cert_path1 = weechat_string_replace (cert_path0, "%h", weechat_dir);
            cert_path2 = (cert_path1) ?
                weechat_string_expand_home (cert_path1) : NULL;

            if (cert_path2)
            {
                cert_str = weechat_file_get_content (cert_path2);
                if (cert_str)
                {
                    weechat_printf (
                        server->buffer,
                        _("%sgnutls: sending one certificate"),
                        weechat_prefix ("network"));

                    filedatum.data = (unsigned char *) cert_str;
                    filedatum.size = strlen (cert_str);

                    /* certificate */
                    gnutls_x509_crt_init (&server->tls_cert);
                    gnutls_x509_crt_import (server->tls_cert, &filedatum,
                                            GNUTLS_X509_FMT_PEM);

                    /* key password */
                    ssl_password = irc_server_eval_expression (
                        server,
                        IRC_SERVER_OPTION_STRING(server,
                                                 IRC_SERVER_OPTION_SSL_PASSWORD));

                    /* key */
                    gnutls_x509_privkey_init (&server->tls_cert_key);

/*
 * gnutls_x509_privkey_import2 has no "Since: ..." in GnuTLS manual but
 * GnuTLS NEWS file lists it being added in 3.1.0:
 * https://gitlab.com/gnutls/gnutls/blob/2b715b9564681acb3008a5574dcf25464de8b038/NEWS#L2552
 */
#if LIBGNUTLS_VERSION_NUMBER >= 0x030100 /* 3.1.0 */
                    ret = gnutls_x509_privkey_import2 (server->tls_cert_key,
                                                       &filedatum,
                                                       GNUTLS_X509_FMT_PEM,
                                                       ssl_password,
                                                       0);
#else
                    ret = gnutls_x509_privkey_import (server->tls_cert_key,
                                                      &filedatum,
                                                      GNUTLS_X509_FMT_PEM);
#endif /* LIBGNUTLS_VERSION_NUMBER >= 0x0301000 */

                    if (ret < 0)
                    {
                        ret = gnutls_x509_privkey_import_pkcs8 (
                            server->tls_cert_key,
                            &filedatum,
                            GNUTLS_X509_FMT_PEM,
                            ssl_password,
                            GNUTLS_PKCS_PLAIN);
                    }
                    if (ret < 0)
                    {
                        weechat_printf (
                            server->buffer,
                            _("%sgnutls: invalid certificate \"%s\", error: "
                              "%s"),
                            weechat_prefix ("error"), cert_path2,
                            gnutls_strerror (ret));
                        rc = -1;
                    }
                    else
                    {

#if LIBGNUTLS_VERSION_NUMBER >= 0x020b00 /* 2.11.0 */
                        tls_struct.cert_type = GNUTLS_CRT_X509;
                        tls_struct.key_type = GNUTLS_PRIVKEY_X509;
#else
                        tls_struct.type = GNUTLS_CRT_X509;
#endif /* LIBGNUTLS_VERSION_NUMBER >= 0x020b00 */
                        tls_struct.ncerts = 1;
                        tls_struct.deinit_all = 0;
                        tls_struct.cert.x509 = &server->tls_cert;
                        tls_struct.key.x509 = server->tls_cert_key;
#if LIBGNUTLS_VERSION_NUMBER >= 0x010706 /* 1.7.6 */
                        /* client certificate info */
#if LIBGNUTLS_VERSION_NUMBER < 0x020400 /* 2.4.0 */
                        rinfo = gnutls_x509_crt_print (server->tls_cert,
                                                       GNUTLS_X509_CRT_ONELINE,
                                                       &cinfo);
#else
                        rinfo = gnutls_x509_crt_print (server->tls_cert,
                                                       GNUTLS_CRT_PRINT_ONELINE,
                                                       &cinfo);
#endif /* LIBGNUTLS_VERSION_NUMBER < 0x020400 */
                        if (rinfo == 0)
                        {
                            weechat_printf (
                                server->buffer,
                                _("%s - client certificate info (%s):"),
                                weechat_prefix ("network"), cert_path2);
                            weechat_printf (
                                server->buffer, "%s  - %s",
                                weechat_prefix ("network"), cinfo.data);
                            gnutls_free (cinfo.data);
                        }
#endif /* LIBGNUTLS_VERSION_NUMBER >= 0x010706 */
                        memcpy (answer, &tls_struct, sizeof (tls_struct));
                        free (cert_str);
                    }

                    if (ssl_password)
                        free (ssl_password);
                }
                else
                {
                    weechat_printf (
                        server->buffer,
                        _("%sgnutls: unable to read certificate \"%s\""),
                        weechat_prefix ("error"), cert_path2);
                }
            }

            if (cert_path1)
                free (cert_path1);
            if (cert_path2)
                free (cert_path2);
        }
    }

end:
    /* an error should stop the handshake unless the user doesn't care */
    if ((rc == -1)
        && (IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_SSL_VERIFY) == 0))
    {
        rc = 0;
    }

    if (cert_temp_init)
        gnutls_x509_crt_deinit (cert_temp);
    if (weechat_dir)
        free (weechat_dir);
    if (fingerprint_eval)
        free (fingerprint_eval);

    return rc;
}