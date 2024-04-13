irc_server_set_buffer_title (struct t_irc_server *server)
{
    char *title;
    int length;

    if (server && server->buffer)
    {
        if (server->is_connected)
        {
            length = 16 +
                ((server->current_address) ? strlen (server->current_address) : 16) +
                16 + ((server->current_ip) ? strlen (server->current_ip) : 16) + 1;
            title = malloc (length);
            if (title)
            {
                snprintf (title, length, "IRC: %s/%d (%s)",
                          server->current_address,
                          server->current_port,
                          (server->current_ip) ? server->current_ip : "");
                weechat_buffer_set (server->buffer, "title", title);
                free (title);
            }
        }
        else
        {
            weechat_buffer_set (server->buffer, "title", "");
        }
    }
}