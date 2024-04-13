irc_server_get_prefix_chars (struct t_irc_server *server)
{
    return (server && server->prefix_chars) ?
        server->prefix_chars : irc_server_prefix_chars_default;
}