irc_nick_realloc_prefixes (struct t_irc_server *server,
                           int old_length, int new_length)
{
    struct t_irc_channel *ptr_channel;
    struct t_irc_nick *ptr_nick;
    char *new_prefixes;

    for (ptr_channel = server->channels; ptr_channel;
         ptr_channel = ptr_channel->next_channel)
    {
        for (ptr_nick = ptr_channel->nicks; ptr_nick;
             ptr_nick = ptr_nick->next_nick)
        {
            if (ptr_nick->prefixes)
            {
                new_prefixes = realloc (ptr_nick->prefixes, new_length + 1);
                if (new_prefixes)
                {
                    ptr_nick->prefixes = new_prefixes;
                    if (new_length > old_length)
                    {
                        memset (ptr_nick->prefixes + old_length,
                                ' ',
                                new_length - old_length);
                    }
                    ptr_nick->prefixes[new_length] = '\0';
                }
            }
            else
            {
                ptr_nick->prefixes = malloc (new_length + 1);
                if (ptr_nick->prefixes)
                {
                    memset (ptr_nick->prefixes, ' ', new_length);
                    ptr_nick->prefixes[new_length] = '\0';
                }
            }
        }
    }
}