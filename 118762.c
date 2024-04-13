irc_server_autojoin_create_buffers (struct t_irc_server *server)
{
    const char *pos_space;
    char *autojoin, *autojoin2, **channels;
    int num_channels, i;

    /* buffers are opened only if no channels are currently opened */
    if (server->channels)
        return;

    /* evaluate server option "autojoin" */
    autojoin = irc_server_eval_expression (
        server,
        IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_AUTOJOIN));

    /* extract channel names from autojoin option */
    if (autojoin && autojoin[0])
    {
        pos_space = strchr (autojoin, ' ');
        autojoin2 = (pos_space) ?
            weechat_strndup (autojoin, pos_space - autojoin) :
            strdup (autojoin);
        if (autojoin2)
        {
            channels = weechat_string_split (
                autojoin2,
                ",",
                NULL,
                WEECHAT_STRING_SPLIT_STRIP_LEFT
                | WEECHAT_STRING_SPLIT_STRIP_RIGHT
                | WEECHAT_STRING_SPLIT_COLLAPSE_SEPS,
                0,
                &num_channels);
            if (channels)
            {
                for (i = 0; i < num_channels; i++)
                {
                    irc_channel_create_buffer (
                        server, IRC_CHANNEL_TYPE_CHANNEL, channels[i],
                        1, 1);
                }
                weechat_string_free_split (channels);
            }
            free (autojoin2);
        }
    }

    if (autojoin)
        free (autojoin);
}