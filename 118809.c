irc_server_set_nick (struct t_irc_server *server, const char *nick)
{
    struct t_irc_channel *ptr_channel;

    /* if nick is the same, just return */
    if ((!server->nick && !nick)
        || (server->nick && nick && strcmp (server->nick, nick) == 0))
    {
        return;
    }

    /* update the nick in server */
    if (server->nick)
        free (server->nick);
    server->nick = (nick) ? strdup (nick) : NULL;

    /* set local variable "nick" for server and all channels/pv */
    weechat_buffer_set (server->buffer, "localvar_set_nick", nick);
    for (ptr_channel = server->channels; ptr_channel;
         ptr_channel = ptr_channel->next_channel)
    {
        weechat_buffer_set (ptr_channel->buffer, "localvar_set_nick", nick);
    }

    weechat_bar_item_update ("input_prompt");
    weechat_bar_item_update ("irc_nick");
    weechat_bar_item_update ("irc_nick_host");
}