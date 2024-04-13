irc_server_check_join_manual_cb (void *data,
                                 struct t_hashtable *hashtable,
                                 const void *key, const void *value)
{
    /* make C compiler happy */
    (void) data;

    if (*((time_t *)value) + (60 * 10) < time (NULL))
        weechat_hashtable_remove (hashtable, key);
}