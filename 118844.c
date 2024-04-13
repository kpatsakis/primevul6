irc_server_sendf (struct t_irc_server *server, int flags, const char *tags,
                  const char *format, ...)
{
    char **items, hash_key[32], value[32], *nick, *command, *channel, *new_msg;
    char str_modifier[128];
    const char *str_message, *str_args;
    int i, items_count, number, ret_number, rc;
    struct t_hashtable *hashtable, *ret_hashtable;

    if (!server)
        return NULL;

    weechat_va_format (format);
    if (!vbuffer)
        return NULL;

    ret_hashtable = NULL;
    ret_number = 1;
    if (flags & IRC_SERVER_SEND_RETURN_HASHTABLE)
    {
        ret_hashtable = weechat_hashtable_new (32,
                                               WEECHAT_HASHTABLE_STRING,
                                               WEECHAT_HASHTABLE_STRING,
                                               NULL, NULL);
    }

    rc = 1;
    items = weechat_string_split (vbuffer, "\n", NULL,
                                  WEECHAT_STRING_SPLIT_STRIP_LEFT
                                  | WEECHAT_STRING_SPLIT_STRIP_RIGHT
                                  | WEECHAT_STRING_SPLIT_COLLAPSE_SEPS,
                                  0, &items_count);
    for (i = 0; i < items_count; i++)
    {
        /* run modifier "irc_out1_xxx" (like "irc_out_xxx", but before split) */
        irc_message_parse (server, items[i], NULL, NULL,
                           &nick, NULL, NULL, &command, &channel, NULL, NULL,
                           NULL, NULL, NULL, NULL);
        snprintf (str_modifier, sizeof (str_modifier),
                  "irc_out1_%s",
                  (command) ? command : "unknown");
        new_msg = weechat_hook_modifier_exec (str_modifier,
                                              server->name,
                                              items[i]);

        /* no changes in new message */
        if (new_msg && (strcmp (items[i], new_msg) == 0))
        {
            free (new_msg);
            new_msg = NULL;
        }

        /* message not dropped? */
        if (!new_msg || new_msg[0])
        {
            /* send signal with command that will be sent to server (before split) */
            irc_server_send_signal (server, "irc_out1",
                                    (command) ? command : "unknown",
                                    (new_msg) ? new_msg : items[i],
                                    NULL);

            /*
             * split message if needed (max is 512 bytes by default,
             * including the final "\r\n")
             */
            hashtable = irc_message_split (server,
                                           (new_msg) ? new_msg : items[i]);
            if (hashtable)
            {
                number = 1;
                while (1)
                {
                    snprintf (hash_key, sizeof (hash_key), "msg%d", number);
                    str_message = weechat_hashtable_get (hashtable, hash_key);
                    if (!str_message)
                        break;
                    snprintf (hash_key, sizeof (hash_key), "args%d", number);
                    str_args = weechat_hashtable_get (hashtable, hash_key);

                    rc = irc_server_send_one_msg (server, flags, str_message,
                                                  nick, command, channel, tags);
                    if (!rc)
                        break;

                    if (ret_hashtable)
                    {
                        snprintf (hash_key, sizeof (hash_key),
                                  "msg%d", ret_number);
                        weechat_hashtable_set (ret_hashtable,
                                               hash_key, str_message);
                        if (str_args)
                        {
                            snprintf (hash_key, sizeof (hash_key),
                                      "args%d", ret_number);
                            weechat_hashtable_set (ret_hashtable,
                                                   hash_key, str_args);
                        }
                        ret_number++;
                    }
                    number++;
                }
                if (ret_hashtable)
                {
                    snprintf (value, sizeof (value), "%d", ret_number - 1);
                    weechat_hashtable_set (ret_hashtable, "count", value);
                }
                weechat_hashtable_free (hashtable);
                if (!rc)
                    break;
            }
        }
        if (nick)
            free (nick);
        if (command)
            free (command);
        if (channel)
            free (channel);
        if (new_msg)
            free (new_msg);
    }
    if (items)
        weechat_string_free_split (items);

    free (vbuffer);

    return ret_hashtable;
}