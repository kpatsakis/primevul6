relay_websocket_save_header (struct t_relay_client *client,
                             const char *message)
{
    char *pos, *name;
    const char *ptr_value;

    /* ignore the "GET" request */
    if (strncmp (message, "GET ", 4) == 0)
        return;

    pos = strchr (message, ':');

    /* not a valid header */
    if (!pos || (pos == message))
        return;

    /* get header name, which is case-insensitive */
    name = weechat_strndup (message, pos - message);
    if (!name)
        return;
    weechat_string_tolower (name);

    /* get pointer on header value */
    ptr_value = pos + 1;
    while (ptr_value[0] == ' ')
    {
        ptr_value++;
    }

    /* add header in the hashtable */
    weechat_hashtable_set (client->http_headers, name, ptr_value);

    free (name);
}