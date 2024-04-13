irc_server_close_connection (struct t_irc_server *server)
{
    int i;

    if (server->hook_timer_connection)
    {
        weechat_unhook (server->hook_timer_connection);
        server->hook_timer_connection = NULL;
    }

    if (server->hook_timer_sasl)
    {
        weechat_unhook (server->hook_timer_sasl);
        server->hook_timer_sasl = NULL;
    }

    if (server->hook_fd)
    {
        weechat_unhook (server->hook_fd);
        server->hook_fd = NULL;
    }

    if (server->hook_connect)
    {
        weechat_unhook (server->hook_connect);
        server->hook_connect = NULL;
    }
    else
    {
#ifdef HAVE_GNUTLS
        /* close SSL connection */
        if (server->ssl_connected)
        {
            if (server->sock != -1)
                gnutls_bye (server->gnutls_sess, GNUTLS_SHUT_WR);
            gnutls_deinit (server->gnutls_sess);
        }
#endif /* HAVE_GNUTLS */
    }
    if (server->sock != -1)
    {
#ifdef _WIN32
        closesocket (server->sock);
#else
        close (server->sock);
#endif /* _WIN32 */
        server->sock = -1;
    }

    /* free any pending message */
    if (server->unterminated_message)
    {
        free (server->unterminated_message);
        server->unterminated_message = NULL;
    }
    for (i = 0; i < IRC_SERVER_NUM_OUTQUEUES_PRIO; i++)
    {
        irc_server_outqueue_free_all (server, i);
    }

    /* remove all redirects */
    irc_redirect_free_all (server);

    /* remove all manual joins */
    weechat_hashtable_remove_all (server->join_manual);

    /* remove all keys for pending joins */
    weechat_hashtable_remove_all (server->join_channel_key);

    /* remove all keys for joins without switch */
    weechat_hashtable_remove_all (server->join_noswitch);

    /* server is now disconnected */
    server->is_connected = 0;
    server->ssl_connected = 0;
}