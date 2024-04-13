irc_nick_hdata_nick_cb (const void *pointer, void *data,
                        const char *hdata_name)
{
    struct t_hdata *hdata;

    /* make C compiler happy */
    (void) pointer;
    (void) data;

    hdata = weechat_hdata_new (hdata_name, "prev_nick", "next_nick",
                               0, 0, NULL, NULL);
    if (hdata)
    {
        WEECHAT_HDATA_VAR(struct t_irc_nick, name, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_nick, host, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_nick, prefixes, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_nick, prefix, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_nick, away, INTEGER, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_nick, account, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_nick, realname, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_nick, color, STRING, 0, NULL, NULL);
        WEECHAT_HDATA_VAR(struct t_irc_nick, prev_nick, POINTER, 0, NULL, hdata_name);
        WEECHAT_HDATA_VAR(struct t_irc_nick, next_nick, POINTER, 0, NULL, hdata_name);
    }
    return hdata;
}