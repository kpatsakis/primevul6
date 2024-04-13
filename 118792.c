irc_server_disconnect_all ()
{
    struct t_irc_server *ptr_server;

    for (ptr_server = irc_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        irc_server_disconnect (ptr_server, 0, 0);
    }
}