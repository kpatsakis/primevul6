irc_server_outqueue_send (struct t_irc_server *server)
{
    time_t time_now;
    char *pos, *tags_to_send;
    int priority, anti_flood;

    time_now = time (NULL);

    /* detect if system clock has been changed (now lower than before) */
    if (server->last_user_message > time_now)
        server->last_user_message = time_now;

    for (priority = 0; priority < IRC_SERVER_NUM_OUTQUEUES_PRIO; priority++)
    {
        switch (priority)
        {
            case 0:
                anti_flood = IRC_SERVER_OPTION_INTEGER(
                    server, IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_HIGH);
                break;
            default:
                anti_flood = IRC_SERVER_OPTION_INTEGER(
                    server, IRC_SERVER_OPTION_ANTI_FLOOD_PRIO_LOW);
                break;
        }
        if (server->outqueue[priority]
            && (time_now >= server->last_user_message + anti_flood))
        {
            if (server->outqueue[priority]->message_before_mod)
            {
                pos = strchr (server->outqueue[priority]->message_before_mod,
                              '\r');
                if (pos)
                    pos[0] = '\0';
                irc_raw_print (server, IRC_RAW_FLAG_SEND,
                               server->outqueue[priority]->message_before_mod);
                if (pos)
                    pos[0] = '\r';
            }
            if (server->outqueue[priority]->message_after_mod)
            {
                pos = strchr (server->outqueue[priority]->message_after_mod,
                              '\r');
                if (pos)
                    pos[0] = '\0';
                irc_raw_print (server, IRC_RAW_FLAG_SEND |
                               ((server->outqueue[priority]->modified) ? IRC_RAW_FLAG_MODIFIED : 0),
                               server->outqueue[priority]->message_after_mod);
                if (pos)
                    pos[0] = '\r';

                /* send signal with command that will be sent to server */
                irc_server_send_signal (
                    server, "irc_out",
                    server->outqueue[priority]->command,
                    server->outqueue[priority]->message_after_mod,
                    NULL);
                tags_to_send = irc_server_get_tags_to_send (
                    server->outqueue[priority]->tags);
                irc_server_send_signal (
                    server, "irc_outtags",
                    server->outqueue[priority]->command,
                    server->outqueue[priority]->message_after_mod,
                    (tags_to_send) ? tags_to_send : "");
                if (tags_to_send)
                    free (tags_to_send);

                /* send command */
                irc_server_send (
                    server, server->outqueue[priority]->message_after_mod,
                    strlen (server->outqueue[priority]->message_after_mod));
                server->last_user_message = time_now;

                /* start redirection if redirect is set */
                if (server->outqueue[priority]->redirect)
                {
                    irc_redirect_init_command (
                        server->outqueue[priority]->redirect,
                        server->outqueue[priority]->message_after_mod);
                }
            }
            irc_server_outqueue_free (server, priority,
                                      server->outqueue[priority]);
            break;
        }
    }
}