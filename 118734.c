irc_nick_color_for_pv (struct t_irc_channel *channel, const char *nickname)
{
    if (weechat_config_boolean (irc_config_look_color_pv_nick_like_channel))
    {
        if (!channel->pv_remote_nick_color)
            channel->pv_remote_nick_color = irc_nick_find_color (nickname);
        if (channel->pv_remote_nick_color)
            return channel->pv_remote_nick_color;
    }

    return IRC_COLOR_CHAT_NICK_OTHER;
}