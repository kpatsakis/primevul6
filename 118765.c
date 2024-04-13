irc_server_remove_away (struct t_irc_server *server)
{
    struct t_irc_channel *ptr_channel;

    if (server->is_connected)
    {
        for (ptr_channel = server->channels; ptr_channel;
             ptr_channel = ptr_channel->next_channel)
        {
            if (ptr_channel->type == IRC_CHANNEL_TYPE_CHANNEL)
                irc_channel_remove_away (server, ptr_channel);
        }
        server->last_away_check = 0;
    }
}