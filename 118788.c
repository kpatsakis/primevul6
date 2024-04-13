irc_nick_find_color (const char *nickname)
{
    return weechat_info_get ("nick_color", nickname);
}