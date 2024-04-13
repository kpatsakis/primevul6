irc_server_recv_cb (const void *pointer, void *data, int fd)
{
    struct t_irc_server *server;
    static char buffer[4096 + 2];
    int num_read, msgq_flush, end_recv;

    /* make C compiler happy */
    (void) data;
    (void) fd;

    server = (struct t_irc_server *)pointer;
    if (!server)
        return WEECHAT_RC_ERROR;

    msgq_flush = 0;
    end_recv = 0;

    while (!end_recv)
    {
        end_recv = 1;

#ifdef HAVE_GNUTLS
        if (server->ssl_connected)
            num_read = gnutls_record_recv (server->gnutls_sess, buffer,
                                           sizeof (buffer) - 2);
        else
#endif /* HAVE_GNUTLS */
            num_read = recv (server->sock, buffer, sizeof (buffer) - 2, 0);

        if (num_read > 0)
        {
            buffer[num_read] = '\0';
            irc_server_msgq_add_buffer (server, buffer);
            msgq_flush = 1;  /* the flush will be done after the loop */
#ifdef HAVE_GNUTLS
            if (server->ssl_connected
                && (gnutls_record_check_pending (server->gnutls_sess) > 0))
            {
                /*
                 * if there are unread data in the gnutls buffers,
                 * go on with recv
                 */
                end_recv = 0;
            }
#endif /* HAVE_GNUTLS */
        }
        else
        {
#ifdef HAVE_GNUTLS
            if (server->ssl_connected)
            {
                if ((num_read == 0)
                    || ((num_read != GNUTLS_E_AGAIN)
                        && (num_read != GNUTLS_E_INTERRUPTED)))
                {
                    weechat_printf (
                        server->buffer,
                        _("%s%s: reading data on socket: error %d %s"),
                        weechat_prefix ("error"), IRC_PLUGIN_NAME,
                        num_read,
                        (num_read == 0) ? _("(connection closed by peer)") :
                        gnutls_strerror (num_read));
                    weechat_printf (
                        server->buffer,
                        _("%s%s: disconnecting from server..."),
                        weechat_prefix ("network"), IRC_PLUGIN_NAME);
                    irc_server_disconnect (server, !server->is_connected, 1);
                }
            }
            else
#endif /* HAVE_GNUTLS */
            {
                if ((num_read == 0)
                    || ((errno != EAGAIN) && (errno != EWOULDBLOCK)))
                {
                    weechat_printf (
                        server->buffer,
                        _("%s%s: reading data on socket: error %d %s"),
                        weechat_prefix ("error"), IRC_PLUGIN_NAME,
                        errno,
                        (num_read == 0) ? _("(connection closed by peer)") :
                        strerror (errno));
                    weechat_printf (
                        server->buffer,
                        _("%s%s: disconnecting from server..."),
                        weechat_prefix ("network"), IRC_PLUGIN_NAME);
                    irc_server_disconnect (server, !server->is_connected, 1);
                }
            }
        }
    }

    if (msgq_flush)
        irc_server_msgq_flush ();

    return WEECHAT_RC_OK;
}