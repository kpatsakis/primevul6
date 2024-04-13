irc_nick_set_current_prefix (struct t_irc_nick *nick)
{
    char *ptr_prefixes;

    if (!nick)
        return;

    nick->prefix[0] = ' ';
    for (ptr_prefixes = nick->prefixes; ptr_prefixes[0]; ptr_prefixes++)
    {
        if (ptr_prefixes[0] != ' ')
        {
            nick->prefix[0] = ptr_prefixes[0];
            break;
        }
    }
}