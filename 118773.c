irc_nick_free_all (struct t_irc_server *server, struct t_irc_channel *channel)
{
    if (!channel)
        return;

    /* remove all nicks for the channel */
    while (channel->nicks)
    {
        irc_nick_free (server, channel, channel->nicks);
    }

    /* remove all groups in nicklist */
    weechat_nicklist_remove_all (channel->buffer);

    /* should be zero, but prevent any bug :D */
    channel->nicks_count = 0;
}