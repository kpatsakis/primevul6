irc_server_get_chanmodes (struct t_irc_server *server)
{
    return (server && server->chanmodes) ?
        server->chanmodes : irc_server_chanmodes_default;
}