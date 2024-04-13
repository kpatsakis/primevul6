irc_server_get_alternate_nick (struct t_irc_server *server)
{
    static char nick[64];
    char str_number[64];
    int nick_index, length_nick, length_number;

    nick[0] = '\0';

    /* we are still trying nicks from option "nicks" */
    if (server->nick_alternate_number < 0)
    {
        nick_index = irc_server_get_nick_index (server);
        if (nick_index < 0)
            nick_index = 0;
        else
        {
            nick_index = (nick_index + 1) % server->nicks_count;
            /* stop loop if first nick tried was not in the list of nicks */
            if ((nick_index == 0) && (server->nick_first_tried < 0))
                server->nick_first_tried = 0;
        }

        if (nick_index != server->nick_first_tried)
        {
            snprintf (nick, sizeof (nick),
                      "%s", server->nicks_array[nick_index]);
            return nick;
        }

        /* now we have tried all nicks in list */

        /* if alternate nicks are disabled, just return NULL */
        if (!IRC_SERVER_OPTION_BOOLEAN(server, IRC_SERVER_OPTION_NICKS_ALTERNATE))
            return NULL;

        /* use main nick and we will add "_" and then number if needed */
        server->nick_alternate_number = 0;
        snprintf (nick, sizeof (nick), "%s", server->nicks_array[0]);
    }
    else
        snprintf (nick, sizeof (nick), "%s", server->nick);

    /* if length is < 9, just add a "_" */
    if (strlen (nick) < 9)
    {
        strcat (nick, "_");
        return nick;
    }

    server->nick_alternate_number++;

    /* number is max 99 */
    if (server->nick_alternate_number > 99)
        return NULL;

    /* be sure the nick has 9 chars max */
    nick[9] = '\0';

    /* generate number */
    snprintf (str_number, sizeof (str_number),
              "%d", server->nick_alternate_number);

    /* copy number in nick */
    length_nick = strlen (nick);
    length_number = strlen (str_number);
    if (length_number > length_nick)
        return NULL;
    memcpy (nick + length_nick - length_number, str_number, length_number);

    /* return alternate nick */
    return nick;
}