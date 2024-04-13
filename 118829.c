irc_nick_set_mode (struct t_irc_server *server, struct t_irc_channel *channel,
                   struct t_irc_nick *nick, int set, char mode)
{
    int index;
    const char *prefix_chars;

    index = irc_server_get_prefix_mode_index (server, mode);
    if (index < 0)
        return;

    /* remove nick from nicklist */
    irc_nick_nicklist_remove (server, channel, nick);

    /* set flag */
    prefix_chars = irc_server_get_prefix_chars (server);
    irc_nick_set_prefix (server, nick, set, prefix_chars[index]);

    /* add nick in nicklist */
    irc_nick_nicklist_add (server, channel, nick);

    if (irc_server_strcasecmp (server, nick->name, server->nick) == 0)
    {
        weechat_bar_item_update ("input_prompt");
        weechat_bar_item_update ("irc_nick");
        weechat_bar_item_update ("irc_nick_host");
    }
}