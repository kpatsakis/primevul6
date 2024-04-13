irc_server_send_signal (struct t_irc_server *server, const char *signal,
                        const char *command, const char *full_message,
                        const char *tags)
{
    int length;
    char *str_signal, *full_message_tags;

    length = strlen (server->name) + 1 + strlen (signal) + 1 + strlen (command) + 1;
    str_signal = malloc (length);
    if (str_signal)
    {
        snprintf (str_signal, length,
                  "%s,%s_%s", server->name, signal, command);
        if (tags)
        {
            length = strlen (tags) + 1 + strlen (full_message) + 1;
            full_message_tags = malloc (length);
            if (full_message_tags)
            {
                snprintf (full_message_tags, length,
                          "%s;%s", tags, full_message);
                (void) weechat_hook_signal_send (str_signal,
                                                 WEECHAT_HOOK_SIGNAL_STRING,
                                                 (void *)full_message_tags);
                free (full_message_tags);
            }
        }
        else
        {
            (void) weechat_hook_signal_send (str_signal,
                                             WEECHAT_HOOK_SIGNAL_STRING,
                                             (void *)full_message);
        }
        free (str_signal);
    }
}