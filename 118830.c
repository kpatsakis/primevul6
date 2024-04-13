irc_server_set_index_current_address (struct t_irc_server *server, int index)
{
    int addresses_changed;

    addresses_changed = irc_server_set_addresses (
        server,
        IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_ADDRESSES));

    if (addresses_changed)
    {
        /* if the addresses have changed, reset the index to 0 */
        index = 0;
    }

    if (server->current_address)
    {
        free (server->current_address);
        server->current_address = NULL;

        /* copy current retry value before loading next server */
        if (!addresses_changed
            && server->index_current_address < server->addresses_count)
        {
            server->retry_array[server->index_current_address] = server->current_retry;
        }
    }
    server->current_port = 0;
    server->current_retry = 0;

    if (server->addresses_count > 0)
    {
        index %= server->addresses_count;
        server->index_current_address = index;
        server->current_address = strdup (server->addresses_array[index]);
        server->current_port = server->ports_array[index];
        server->current_retry = server->retry_array[index];
    }
}