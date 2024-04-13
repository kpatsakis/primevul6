relay_websocket_is_http_get_weechat (const char *message)
{
    /* the message must start with "GET /weechat" */
    if (strncmp (message, "GET /weechat", 12) != 0)
        return 0;

    /* after "GET /weechat", only a new line or " HTTP" is allowed */
    if ((message[12] != '\r') && (message[12] != '\n')
        && (strncmp (message + 12, " HTTP", 5) != 0))
    {
        return 0;
    }

    /* valid HTTP GET for resource "/weechat" */
    return 1;
}