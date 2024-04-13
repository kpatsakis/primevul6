irc_nick_free (struct t_irc_server *server, struct t_irc_channel *channel,
               struct t_irc_nick *nick)
{
    struct t_irc_nick *new_nicks;

    if (!channel || !nick)
        return;

    /* remove nick from nicklist */
    irc_nick_nicklist_remove (server, channel, nick);

    /* remove nick */
    if (channel->last_nick == nick)
        channel->last_nick = nick->prev_nick;
    if (nick->prev_nick)
    {
        (nick->prev_nick)->next_nick = nick->next_nick;
        new_nicks = channel->nicks;
    }
    else
        new_nicks = nick->next_nick;

    if (nick->next_nick)
        (nick->next_nick)->prev_nick = nick->prev_nick;

    channel->nicks_count--;

    /* free data */
    if (nick->name)
        free (nick->name);
    if (nick->host)
        free (nick->host);
    if (nick->prefixes)
        free (nick->prefixes);
    if (nick->prefix)
        free (nick->prefix);
    if (nick->account)
        free (nick->account);
    if (nick->realname)
        free (nick->realname);
    if (nick->color)
        free (nick->color);

    free (nick);

    channel->nicks = new_nicks;
    channel->nick_completion_reset = 1;
}