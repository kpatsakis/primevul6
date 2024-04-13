relay_websocket_send_http (struct t_relay_client *client,
                           const char *http)
{
    char *message;
    int length;

    length = 32 + strlen (http) + 1;
    message = malloc (length);
    if (message)
    {
        snprintf (message, length, "HTTP/1.1 %s\r\n\r\n", http);
        relay_client_send (client, RELAY_CLIENT_MSG_STANDARD,
                           message, strlen (message), NULL);
        free (message);
    }
}