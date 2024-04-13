irc_nick_set_prefixes (struct t_irc_server *server, struct t_irc_nick *nick,
                       const char *prefixes)
{
    const char *ptr_prefixes;

    if (!nick)
        return;

    /* reset all prefixes in nick */
    memset (nick->prefixes, ' ', strlen (nick->prefixes));

    /* add prefixes in nick */
    if (prefixes)
    {
        for (ptr_prefixes = prefixes; ptr_prefixes[0]; ptr_prefixes++)
        {
            irc_nick_set_prefix (server, nick, 1, ptr_prefixes[0]);
        }
    }

    /* set current prefix */
    irc_nick_set_current_prefix (nick);
}