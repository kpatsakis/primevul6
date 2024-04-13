irc_server_msgq_add_unterminated (struct t_irc_server *server,
                                  const char *string)
{
    char *unterminated_message2;

    if (!string[0])
        return;

    if (server->unterminated_message)
    {
        unterminated_message2 =
            realloc (server->unterminated_message,
                     (strlen (server->unterminated_message) +
                      strlen (string) + 1));
        if (!unterminated_message2)
        {
            weechat_printf (server->buffer,
                            _("%s%s: not enough memory for received message"),
                            weechat_prefix ("error"), IRC_PLUGIN_NAME);
            free (server->unterminated_message);
            server->unterminated_message = NULL;
            return;
        }
        server->unterminated_message = unterminated_message2;
        strcat (server->unterminated_message, string);
    }
    else
    {
        server->unterminated_message = strdup (string);
        if (!server->unterminated_message)
        {
            weechat_printf (server->buffer,
                            _("%s%s: not enough memory for received message"),
                            weechat_prefix ("error"), IRC_PLUGIN_NAME);
        }
    }
}