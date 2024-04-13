irc_nick_find_color_name (const char *nickname)
{
    return weechat_info_get ("nick_color_name", nickname);
}