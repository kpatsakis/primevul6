irc_server_get_prefix_modes (struct t_irc_server *server)
{
    return (server && server->prefix_modes) ?
        server->prefix_modes : irc_server_prefix_modes_default;
}