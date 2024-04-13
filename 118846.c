irc_server_xfer_resume_ready_cb (const void *pointer, void *data,
                                 const char *signal,
                                 const char *type_data, void *signal_data)
{
    struct t_infolist *infolist;
    struct t_irc_server *ptr_server;
    const char *plugin_name, *plugin_id, *filename;
    int spaces_in_name;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    infolist = (struct t_infolist *)signal_data;

    if (weechat_infolist_next (infolist))
    {
        plugin_name = weechat_infolist_string (infolist, "plugin_name");
        plugin_id = weechat_infolist_string (infolist, "plugin_id");
        if (plugin_name && (strcmp (plugin_name, IRC_PLUGIN_NAME) == 0) && plugin_id)
        {
            ptr_server = irc_server_search (plugin_id);
            if (ptr_server)
            {
                filename = weechat_infolist_string (infolist, "filename");
                spaces_in_name = (strchr (filename, ' ') != NULL);
                irc_server_sendf (
                    ptr_server,
                    IRC_SERVER_SEND_OUTQ_PRIO_HIGH, NULL,
                    "PRIVMSG %s :\01DCC RESUME %s%s%s %d %s\01",
                    weechat_infolist_string (infolist, "remote_nick"),
                    (spaces_in_name) ? "\"" : "",
                    filename,
                    (spaces_in_name) ? "\"" : "",
                    weechat_infolist_integer (infolist, "port"),
                    weechat_infolist_string (infolist, "start_resume"));
            }
        }
    }

    weechat_infolist_reset_item_cursor (infolist);

    return WEECHAT_RC_OK;
}