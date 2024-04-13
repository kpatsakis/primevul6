irc_nick_new (struct t_irc_server *server, struct t_irc_channel *channel,
              const char *nickname, const char *host, const char *prefixes,
              int away, const char *account, const char *realname)
{
    struct t_irc_nick *new_nick, *ptr_nick;
    int length;

    if (!nickname || !nickname[0])
        return NULL;

    if (!channel->nicks)
        irc_channel_add_nicklist_groups (server, channel);

    /* nick already exists on this channel? */
    ptr_nick = irc_nick_search (server, channel, nickname);
    if (ptr_nick)
    {
        /* remove old nick from nicklist */
        irc_nick_nicklist_remove (server, channel, ptr_nick);

        /* update nick prefixes */
        irc_nick_set_prefixes (server, ptr_nick, prefixes);

        /* add new nick in nicklist */
        irc_nick_nicklist_add (server, channel, ptr_nick);

        return ptr_nick;
    }

    /* alloc memory for new nick */
    if ((new_nick = malloc (sizeof (*new_nick))) == NULL)
        return NULL;

    /* initialize new nick */
    new_nick->name = strdup (nickname);
    new_nick->host = (host) ? strdup (host) : NULL;
    new_nick->account = (account) ? strdup (account) : NULL;
    new_nick->realname = (realname) ? strdup (realname) : NULL;
    length = strlen (irc_server_get_prefix_chars (server));
    new_nick->prefixes = malloc (length + 1);
    new_nick->prefix = malloc (2);
    if (!new_nick->name || !new_nick->prefixes || !new_nick->prefix)
    {
        if (new_nick->name)
            free (new_nick->name);
        if (new_nick->host)
            free (new_nick->host);
        if (new_nick->account)
            free (new_nick->account);
        if (new_nick->realname)
            free (new_nick->realname);
        if (new_nick->prefixes)
            free (new_nick->prefixes);
        if (new_nick->prefix)
            free (new_nick->prefix);
        free (new_nick);
        return NULL;
    }
    memset (new_nick->prefixes, ' ', length);
    new_nick->prefixes[length] = '\0';
    new_nick->prefix[0] = ' ';
    new_nick->prefix[1] = '\0';
    irc_nick_set_prefixes (server, new_nick, prefixes);
    new_nick->away = away;
    if (irc_server_strcasecmp (server, new_nick->name, server->nick) == 0)
        new_nick->color = strdup (IRC_COLOR_CHAT_NICK_SELF);
    else
        new_nick->color = irc_nick_find_color (new_nick->name);

    /* add nick to end of list */
    new_nick->prev_nick = channel->last_nick;
    if (channel->last_nick)
        channel->last_nick->next_nick = new_nick;
    else
        channel->nicks = new_nick;
    channel->last_nick = new_nick;
    new_nick->next_nick = NULL;

    channel->nicks_count++;

    channel->nick_completion_reset = 1;

    /* add nick to buffer nicklist */
    irc_nick_nicklist_add (server, channel, new_nick);

    /* all is OK, return address of new nick */
    return new_nick;
}