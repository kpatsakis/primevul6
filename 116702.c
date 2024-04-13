relay_websocket_client_handshake_valid (struct t_relay_client *client)
{
    const char *value;

    /* check if we have header "Upgrade" with value "websocket" */
    value = weechat_hashtable_get (client->http_headers, "upgrade");
    if (!value)
        return -1;
    if (weechat_strcasecmp (value, "websocket") != 0)
        return -1;

    /* check if we have header "Sec-WebSocket-Key" with non-empty value */
    value = weechat_hashtable_get (client->http_headers, "sec-websocket-key");
    if (!value || !value[0])
        return -1;

    if (relay_config_regex_websocket_allowed_origins)
    {
        value = weechat_hashtable_get (client->http_headers, "origin");
        if (!value || !value[0])
            return -2;
        if (regexec (relay_config_regex_websocket_allowed_origins, value, 0,
                     NULL, 0) != 0)
        {
            return -2;
        }
    }

    /* client handshake is valid */
    return 0;
}