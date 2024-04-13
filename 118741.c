irc_nick_default_ban_mask (struct t_irc_nick *nick)
{
    const char *ptr_ban_mask;
    char *pos_hostname, user[128], ident[128], *res, *temp;

    if (!nick)
        return NULL;

    ptr_ban_mask = weechat_config_string (irc_config_network_ban_mask_default);

    pos_hostname = (nick->host) ? strchr (nick->host, '@') : NULL;

    if (!nick->host || !pos_hostname || !ptr_ban_mask || !ptr_ban_mask[0])
        return NULL;

    if (pos_hostname - nick->host > (int)sizeof (user) - 1)
        return NULL;

    strncpy (user, nick->host, pos_hostname - nick->host);
    user[pos_hostname - nick->host] = '\0';
    strcpy (ident, (user[0] != '~') ? user : "*");
    pos_hostname++;

    /* replace nick */
    temp = weechat_string_replace (ptr_ban_mask, "$nick", nick->name);
    if (!temp)
        return NULL;
    res = temp;

    /* replace user */
    temp = weechat_string_replace (res, "$user", user);
    free (res);
    if (!temp)
        return NULL;
    res = temp;

    /* replace ident */
    temp = weechat_string_replace (res, "$ident", ident);
    free (res);
    if (!temp)
        return NULL;
    res = temp;

    /* replace hostname */
    temp = weechat_string_replace (res, "$host", pos_hostname);
    free (res);
    if (!temp)
        return NULL;
    res = temp;

    return res;
}