irc_server_set_away (struct t_irc_server *server, const char *nick, int is_away)
{
    struct t_irc_channel *ptr_channel;

    if (server->is_connected)
    {
        /* set/del "away" local variable on server buffer */
        if (is_away)
        {
            weechat_buffer_set (server->buffer,
                                "localvar_set_away", server->away_message);
        }
        else
        {
            weechat_buffer_set (server->buffer,
                                "localvar_del_away", "");
        }

        for (ptr_channel = server->channels; ptr_channel;
             ptr_channel = ptr_channel->next_channel)
        {
            /* set away flag for nick on channel */
            if (ptr_channel->type == IRC_CHANNEL_TYPE_CHANNEL)
                irc_channel_set_away (server, ptr_channel, nick, is_away);

            /* set/del "away" local variable on channel buffer */
            if (is_away)
            {
                weechat_buffer_set (ptr_channel->buffer,
                                    "localvar_set_away", server->away_message);
            }
            else
            {
                weechat_buffer_set (ptr_channel->buffer,
                                    "localvar_del_away", "");
            }
        }
    }
}