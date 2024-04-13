irc_nick_valid (struct t_irc_channel *channel, struct t_irc_nick *nick)
{
    struct t_irc_nick *ptr_nick;

    if (!channel || !nick)
        return 0;

    for (ptr_nick = channel->nicks; ptr_nick; ptr_nick = ptr_nick->next_nick)
    {
        if (ptr_nick == nick)
            return 1;
    }

    /* nick not found */
    return 0;
}