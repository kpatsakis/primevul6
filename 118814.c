irc_nick_count (struct t_irc_server *server, struct t_irc_channel *channel,
                int *total, int *count_op, int *count_halfop, int *count_voice,
                int *count_normal)
{
    struct t_irc_nick *ptr_nick;

    (*total) = 0;
    (*count_op) = 0;
    (*count_halfop) = 0;
    (*count_voice) = 0;
    (*count_normal) = 0;
    for (ptr_nick = channel->nicks; ptr_nick;
         ptr_nick = ptr_nick->next_nick)
    {
        (*total)++;
        if (irc_nick_is_op (server, ptr_nick))
            (*count_op)++;
        else
        {
            if (irc_nick_has_prefix_mode (server, ptr_nick, 'h'))
                (*count_halfop)++;
            else
            {
                if (irc_nick_has_prefix_mode (server, ptr_nick, 'v'))
                    (*count_voice)++;
                else
                    (*count_normal)++;
            }
        }
    }
}