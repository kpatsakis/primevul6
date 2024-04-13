irc_nick_search (struct t_irc_server *server, struct t_irc_channel *channel,
                 const char *nickname)
{
    struct t_irc_nick *ptr_nick;

    if (!channel || !nickname)
        return NULL;

    for (ptr_nick = channel->nicks; ptr_nick;
         ptr_nick = ptr_nick->next_nick)
    {
        if (irc_server_strcasecmp (server, ptr_nick->name, nickname) == 0)
            return ptr_nick;
    }

    /* nick not found */
    return NULL;
}