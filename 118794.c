irc_server_reorder (const char **servers, int num_servers)
{
    struct t_irc_server *ptr_server, *ptr_server2;
    int i, num_moved;

    ptr_server = irc_servers;
    num_moved = 0;

    for (i = 0; ptr_server && (i < num_servers); i++)
    {
        for (ptr_server2 = ptr_server; ptr_server2;
             ptr_server2 = ptr_server2->next_server)
        {
            if (strcmp (ptr_server2->name, servers[i]) == 0)
                break;
        }
        if (ptr_server2 == ptr_server)
        {
            ptr_server = ptr_server->next_server;
        }
        else  if (ptr_server2)
        {
            /* extract server from list */
            if (ptr_server2 == irc_servers)
                irc_servers = ptr_server2->next_server;
            if (ptr_server2 == last_irc_server)
                last_irc_server = ptr_server2->prev_server;
            if (ptr_server2->prev_server)
                (ptr_server2->prev_server)->next_server = ptr_server2->next_server;
            if (ptr_server2->next_server)
                (ptr_server2->next_server)->prev_server = ptr_server2->prev_server;

            /* set pointers in ptr_server2 */
            ptr_server2->prev_server = ptr_server->prev_server;
            ptr_server2->next_server = ptr_server;

            /* insert ptr_server2 before ptr_server */
            if (ptr_server->prev_server)
                (ptr_server->prev_server)->next_server = ptr_server2;
            ptr_server->prev_server = ptr_server2;

            /* adjust list of servers if needed */
            if (ptr_server == irc_servers)
                irc_servers = ptr_server2;

            num_moved++;
        }
    }

    return num_moved;
}