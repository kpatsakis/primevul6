irc_server_send (struct t_irc_server *server, const char *buffer, int size_buf)
{
    int rc;

    if (!server)
    {
        weechat_printf (
            NULL,
            _("%s%s: sending data to server: null pointer (please report "
              "problem to developers)"),
            weechat_prefix ("error"), IRC_PLUGIN_NAME);
        return 0;
    }

    if (size_buf <= 0)
    {
        weechat_printf (
            server->buffer,
            _("%s%s: sending data to server: empty buffer (please report "
              "problem to developers)"),
            weechat_prefix ("error"), IRC_PLUGIN_NAME);
        return 0;
    }

#ifdef HAVE_GNUTLS
    if (server->ssl_connected)
        rc = gnutls_record_send (server->gnutls_sess, buffer, size_buf);
    else
#endif /* HAVE_GNUTLS */
        rc = send (server->sock, buffer, size_buf, 0);

    if (rc < 0)
    {
#ifdef HAVE_GNUTLS
        if (server->ssl_connected)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: sending data to server: error %d %s"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME,
                rc, gnutls_strerror (rc));
        }
        else
#endif /* HAVE_GNUTLS */
        {
            weechat_printf (
                server->buffer,
                _("%s%s: sending data to server: error %d %s"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME,
                errno, strerror (errno));
        }
    }

    return rc;
}