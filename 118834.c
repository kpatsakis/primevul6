irc_server_connect_cb (const void *pointer, void *data,
                       int status, int gnutls_rc, int sock,
                       const char *error, const char *ip_address)
{
    struct t_irc_server *server;
    const char *proxy;

    /* make C compiler happy */
    (void) data;

    server = (struct t_irc_server *)pointer;

    proxy = IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_PROXY);

    server->hook_connect = NULL;

    server->sock = sock;

    switch (status)
    {
        case WEECHAT_HOOK_CONNECT_OK:
            /* set IP */
            if (server->current_ip)
                free (server->current_ip);
            server->current_ip = (ip_address) ? strdup (ip_address) : NULL;
            weechat_printf (
                server->buffer,
                _("%s%s: connected to %s/%d (%s)"),
                weechat_prefix ("network"),
                IRC_PLUGIN_NAME,
                server->current_address,
                server->current_port,
                (server->current_ip) ? server->current_ip : "?");
            server->hook_fd = weechat_hook_fd (server->sock,
                                               1, 0, 0,
                                               &irc_server_recv_cb,
                                               server, NULL);
            /* login to server */
            irc_server_login (server);
            break;
        case WEECHAT_HOOK_CONNECT_ADDRESS_NOT_FOUND:
            weechat_printf (
                server->buffer,
                (proxy && proxy[0]) ?
                _("%s%s: proxy address \"%s\" not found") :
                _("%s%s: address \"%s\" not found"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME,
                server->current_address);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            irc_server_switch_address (server, 1);
            break;
        case WEECHAT_HOOK_CONNECT_IP_ADDRESS_NOT_FOUND:
            weechat_printf (
                server->buffer,
                (proxy && proxy[0]) ?
                _("%s%s: proxy IP address not found") :
                _("%s%s: IP address not found"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            irc_server_switch_address (server, 1);
            break;
        case WEECHAT_HOOK_CONNECT_CONNECTION_REFUSED:
            weechat_printf (
                server->buffer,
                (proxy && proxy[0]) ?
                _("%s%s: proxy connection refused") :
                _("%s%s: connection refused"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            server->current_retry++;
            irc_server_switch_address (server, 1);
            break;
        case WEECHAT_HOOK_CONNECT_PROXY_ERROR:
            weechat_printf (
                server->buffer,
                _("%s%s: proxy fails to establish connection to server (check "
                  "username/password if used and if server address/port is "
                  "allowed by proxy)"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            irc_server_switch_address (server, 1);
            break;
        case WEECHAT_HOOK_CONNECT_LOCAL_HOSTNAME_ERROR:
            weechat_printf (
                server->buffer,
                _("%s%s: unable to set local hostname/IP"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            irc_server_reconnect_schedule (server);
            break;
        case WEECHAT_HOOK_CONNECT_GNUTLS_INIT_ERROR:
            weechat_printf (
                server->buffer,
                _("%s%s: TLS init error"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            server->current_retry++;
            irc_server_reconnect_schedule (server);
            break;
        case WEECHAT_HOOK_CONNECT_GNUTLS_HANDSHAKE_ERROR:
            weechat_printf (
                server->buffer,
                _("%s%s: TLS handshake failed"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
#ifdef HAVE_GNUTLS
            if (gnutls_rc == GNUTLS_E_DH_PRIME_UNACCEPTABLE)
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: you should play with option "
                      "irc.server.%s.ssl_dhkey_size (current value is %d, try "
                      "a lower value like %d or %d)"),
                    weechat_prefix ("error"),
                    IRC_PLUGIN_NAME,
                    server->name,
                    IRC_SERVER_OPTION_INTEGER (
                        server, IRC_SERVER_OPTION_SSL_DHKEY_SIZE),
                    IRC_SERVER_OPTION_INTEGER (
                        server, IRC_SERVER_OPTION_SSL_DHKEY_SIZE) / 2,
                    IRC_SERVER_OPTION_INTEGER (
                        server, IRC_SERVER_OPTION_SSL_DHKEY_SIZE) / 4);
            }
#else
            (void) gnutls_rc;
#endif /* HAVE_GNUTLS */
            irc_server_close_connection (server);
            server->current_retry++;
            irc_server_switch_address (server, 1);
            break;
        case WEECHAT_HOOK_CONNECT_MEMORY_ERROR:
            weechat_printf (
                server->buffer,
                _("%s%s: not enough memory (%s)"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME,
                (error) ? error : "-");
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            irc_server_reconnect_schedule (server);
            break;
        case WEECHAT_HOOK_CONNECT_TIMEOUT:
            weechat_printf (
                server->buffer,
                _("%s%s: timeout"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            server->current_retry++;
            irc_server_switch_address (server, 1);
            break;
        case WEECHAT_HOOK_CONNECT_SOCKET_ERROR:
            weechat_printf (
                server->buffer,
                _("%s%s: unable to create socket"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME);
            if (error && error[0])
            {
                weechat_printf (
                    server->buffer,
                    _("%s%s: error: %s"),
                    weechat_prefix ("error"), IRC_PLUGIN_NAME, error);
            }
            irc_server_close_connection (server);
            server->current_retry++;
            irc_server_reconnect_schedule (server);
            break;
    }

    return WEECHAT_RC_OK;
}