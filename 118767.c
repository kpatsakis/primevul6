irc_server_set_addresses (struct t_irc_server *server, const char *addresses)
{
    int i;
    char *pos, *error, *addresses_eval;
    long number;

    addresses_eval = NULL;

    if (addresses && addresses[0])
    {
        addresses_eval = irc_server_eval_expression (server, addresses);
        if (server->addresses_eval
            && (strcmp (server->addresses_eval, addresses_eval) == 0))
        {
            free (addresses_eval);
            return 0;
        }
    }

    /* free data */
    if (server->addresses_eval)
    {
        free (server->addresses_eval);
        server->addresses_eval = NULL;
    }
    server->addresses_count = 0;
    if (server->addresses_array)
    {
        weechat_string_free_split (server->addresses_array);
        server->addresses_array = NULL;
    }
    if (server->ports_array)
    {
        free (server->ports_array);
        server->ports_array = NULL;
    }
    if (server->retry_array)
    {
        free (server->retry_array);
        server->retry_array = NULL;
    }

    /* set new addresses/ports */
    server->addresses_eval = addresses_eval;
    if (!addresses_eval)
        return 1;
    server->addresses_array = weechat_string_split (
        addresses_eval,
        ",",
        " ",
        WEECHAT_STRING_SPLIT_STRIP_LEFT
        | WEECHAT_STRING_SPLIT_STRIP_RIGHT
        | WEECHAT_STRING_SPLIT_COLLAPSE_SEPS,
        0,
        &server->addresses_count);
    server->ports_array = malloc (
        server->addresses_count * sizeof (server->ports_array[0]));
    server->retry_array = malloc (
        server->addresses_count * sizeof (server->retry_array[0]));
    for (i = 0; i < server->addresses_count; i++)
    {
        pos = strchr (server->addresses_array[i], '/');
        if (pos)
        {
            pos[0] = 0;
            pos++;
            error = NULL;
            number = strtol (pos, &error, 10);
            server->ports_array[i] = (error && !error[0]) ?
                number : IRC_SERVER_DEFAULT_PORT;
        }
        else
        {
            server->ports_array[i] = IRC_SERVER_DEFAULT_PORT;
        }
        server->retry_array[i] = 0;
    }

    return 1;
}