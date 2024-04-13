irc_server_msgq_flush ()
{
    struct t_irc_message *next;
    char *ptr_data, *new_msg, *new_msg2, *ptr_msg, *ptr_msg2, *pos;
    char *nick, *host, *command, *channel, *arguments;
    char *msg_decoded, *msg_decoded_without_color;
    char str_modifier[128], modifier_data[256];
    int pos_channel, pos_text, pos_decode;

    while (irc_recv_msgq)
    {
        if (irc_recv_msgq->data)
        {
            /* read message only if connection was not lost */
            if (irc_recv_msgq->server->sock != -1)
            {
                ptr_data = irc_recv_msgq->data;
                while (ptr_data[0] == ' ')
                {
                    ptr_data++;
                }

                if (ptr_data[0])
                {
                    irc_raw_print (irc_recv_msgq->server, IRC_RAW_FLAG_RECV,
                                   ptr_data);

                    irc_message_parse (irc_recv_msgq->server,
                                       ptr_data, NULL, NULL, NULL, NULL, NULL,
                                       &command, NULL, NULL, NULL, NULL, NULL,
                                       NULL, NULL);
                    snprintf (str_modifier, sizeof (str_modifier),
                              "irc_in_%s",
                              (command) ? command : "unknown");
                    new_msg = weechat_hook_modifier_exec (
                        str_modifier,
                        irc_recv_msgq->server->name,
                        ptr_data);
                    if (command)
                        free (command);

                    /* no changes in new message */
                    if (new_msg && (strcmp (ptr_data, new_msg) == 0))
                    {
                        free (new_msg);
                        new_msg = NULL;
                    }

                    /* message not dropped? */
                    if (!new_msg || new_msg[0])
                    {
                        /* use new message (returned by plugin) */
                        ptr_msg = (new_msg) ? new_msg : ptr_data;

                        while (ptr_msg && ptr_msg[0])
                        {
                            pos = strchr (ptr_msg, '\n');
                            if (pos)
                                pos[0] = '\0';

                            if (new_msg)
                            {
                                irc_raw_print (
                                    irc_recv_msgq->server,
                                    IRC_RAW_FLAG_RECV | IRC_RAW_FLAG_MODIFIED,
                                    ptr_msg);
                            }

                            irc_message_parse (irc_recv_msgq->server, ptr_msg,
                                               NULL, NULL, &nick, NULL, &host,
                                               &command, &channel, &arguments,
                                               NULL, NULL, NULL,
                                               &pos_channel, &pos_text);

                            msg_decoded = NULL;


                            switch (IRC_SERVER_OPTION_INTEGER(irc_recv_msgq->server,
                                                              IRC_SERVER_OPTION_CHARSET_MESSAGE))
                            {
                                case IRC_SERVER_CHARSET_MESSAGE_MESSAGE:
                                    pos_decode = 0;
                                    break;
                                case IRC_SERVER_CHARSET_MESSAGE_CHANNEL:
                                    pos_decode = (pos_channel >= 0) ? pos_channel : pos_text;
                                    break;
                                case IRC_SERVER_CHARSET_MESSAGE_TEXT:
                                    pos_decode = pos_text;
                                    break;
                                default:
                                    pos_decode = 0;
                                    break;
                            }
                            if (pos_decode >= 0)
                            {
                                /* convert charset for message */
                                if (channel
                                    && irc_channel_is_channel (irc_recv_msgq->server,
                                                               channel))
                                {
                                    snprintf (modifier_data, sizeof (modifier_data),
                                              "%s.%s.%s",
                                              weechat_plugin->name,
                                              irc_recv_msgq->server->name,
                                              channel);
                                }
                                else
                                {
                                    if (nick && (!host || (strcmp (nick, host) != 0)))
                                    {
                                        snprintf (modifier_data,
                                                  sizeof (modifier_data),
                                                  "%s.%s.%s",
                                                  weechat_plugin->name,
                                                  irc_recv_msgq->server->name,
                                                  nick);
                                    }
                                    else
                                    {
                                        snprintf (modifier_data,
                                                  sizeof (modifier_data),
                                                  "%s.%s",
                                                  weechat_plugin->name,
                                                  irc_recv_msgq->server->name);
                                    }
                                }
                                msg_decoded = irc_message_convert_charset (
                                    ptr_msg, pos_decode,
                                    "charset_decode", modifier_data);
                            }

                            /* replace WeeChat internal color codes by "?" */
                            msg_decoded_without_color =
                                weechat_string_remove_color (
                                    (msg_decoded) ? msg_decoded : ptr_msg,
                                    "?");

                            /* call modifier after charset */
                            ptr_msg2 = (msg_decoded_without_color) ?
                                msg_decoded_without_color : ((msg_decoded) ? msg_decoded : ptr_msg);
                            snprintf (str_modifier, sizeof (str_modifier),
                                      "irc_in2_%s",
                                      (command) ? command : "unknown");
                            new_msg2 = weechat_hook_modifier_exec (
                                str_modifier,
                                irc_recv_msgq->server->name,
                                ptr_msg2);
                            if (new_msg2 && (strcmp (ptr_msg2, new_msg2) == 0))
                            {
                                free (new_msg2);
                                new_msg2 = NULL;
                            }

                            /* message not dropped? */
                            if (!new_msg2 || new_msg2[0])
                            {
                                /* use new message (returned by plugin) */
                                if (new_msg2)
                                    ptr_msg2 = new_msg2;

                                /* parse and execute command */
                                if (irc_redirect_message (irc_recv_msgq->server,
                                                          ptr_msg2, command,
                                                          arguments))
                                {
                                    /* message redirected, we'll not display it! */
                                }
                                else
                                {
                                    /* message not redirected, display it */
                                    irc_protocol_recv_command (
                                        irc_recv_msgq->server,
                                        ptr_msg2,
                                        command,
                                        channel);
                                }
                            }

                            if (new_msg2)
                                free (new_msg2);
                            if (nick)
                                free (nick);
                            if (host)
                                free (host);
                            if (command)
                                free (command);
                            if (channel)
                                free (channel);
                            if (arguments)
                                free (arguments);
                            if (msg_decoded)
                                free (msg_decoded);
                            if (msg_decoded_without_color)
                                free (msg_decoded_without_color);

                            if (pos)
                            {
                                pos[0] = '\n';
                                ptr_msg = pos + 1;
                            }
                            else
                                ptr_msg = NULL;
                        }
                    }
                    else
                    {
                        irc_raw_print (irc_recv_msgq->server,
                                       IRC_RAW_FLAG_RECV | IRC_RAW_FLAG_MODIFIED,
                                       _("(message dropped)"));
                    }
                    if (new_msg)
                        free (new_msg);
                }
            }
            free (irc_recv_msgq->data);
        }

        next = irc_recv_msgq->next_message;
        free (irc_recv_msgq);
        irc_recv_msgq = next;
        if (!irc_recv_msgq)
            irc_msgq_last_msg = NULL;
    }
}