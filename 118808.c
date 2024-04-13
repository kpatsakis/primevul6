irc_server_get_pv_count (struct t_irc_server *server)
{
    int count;
    struct t_irc_channel *ptr_channel;

    count = 0;
    for (ptr_channel = server->channels; ptr_channel;
         ptr_channel = ptr_channel->next_channel)
    {
        if (ptr_channel->type == IRC_CHANNEL_TYPE_PRIVATE)
            count++;
    }
    return count;
}