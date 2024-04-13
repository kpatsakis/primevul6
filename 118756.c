irc_nick_add_to_infolist (struct t_infolist *infolist,
                          struct t_irc_nick *nick)
{
    struct t_infolist_item *ptr_item;

    if (!infolist || !nick)
        return 0;

    ptr_item = weechat_infolist_new_item (infolist);
    if (!ptr_item)
        return 0;

    if (!weechat_infolist_new_var_string (ptr_item, "name", nick->name))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "host", nick->host))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "prefixes", nick->prefixes))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "prefix", nick->prefix))
        return 0;
    if (!weechat_infolist_new_var_integer (ptr_item, "away", nick->away))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "account", nick->account))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "realname", nick->realname))
        return 0;
    if (!weechat_infolist_new_var_string (ptr_item, "color", nick->color))
        return 0;

    return 1;
}