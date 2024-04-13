irc_server_check_join_smart_filtered_cb (void *data,
                                         struct t_hashtable *hashtable,
                                         const void *key, const void *value)
{
    int unmask_delay;

    /* make C compiler happy */
    (void) data;

    unmask_delay = weechat_config_integer (irc_config_look_smart_filter_join_unmask);
    if ((unmask_delay == 0)
        || (*((time_t *)value) < time (NULL) - (unmask_delay * 60)))
    {
        weechat_hashtable_remove (hashtable, key);
    }
}