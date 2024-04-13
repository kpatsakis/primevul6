irc_nick_nicklist_set_prefix_color_all ()
{
    struct t_irc_server *ptr_server;
    struct t_irc_channel *ptr_channel;
    struct t_irc_nick *ptr_nick;

    for (ptr_server = irc_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        for (ptr_channel = ptr_server->channels; ptr_channel;
             ptr_channel = ptr_channel->next_channel)
        {
            for (ptr_nick = ptr_channel->nicks; ptr_nick;
                 ptr_nick = ptr_nick->next_nick)
            {
                irc_nick_nicklist_set (ptr_channel, ptr_nick, "prefix_color",
                                       irc_nick_get_prefix_color_name (ptr_server,
                                                                       ptr_nick->prefix[0]));
            }
        }
    }
}