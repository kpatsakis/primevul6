irc_server_get_nick_index (struct t_irc_server *server)
{
    int i;

    if (!server->nick)
        return -1;

    for (i = 0; i < server->nicks_count; i++)
    {
        if (strcmp (server->nick, server->nicks_array[i]) == 0)
        {
            return i;
        }
    }

    /* nick not found */
    return -1;
}