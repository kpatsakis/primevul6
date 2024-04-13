irc_server_xfer_send_ready_cb (const void *pointer, void *data,
                               const char *signal,
                               const char *type_data, void *signal_data)
{
    struct t_infolist *infolist;
    struct t_irc_server *ptr_server;
    const char *plugin_name, *plugin_id, *type, *filename, *local_address;
    char converted_addr[NI_MAXHOST];
    struct addrinfo *ainfo;
    struct sockaddr_in *saddr;
    int spaces_in_name, rc;

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
        if (plugin_name && (strcmp (plugin_name, IRC_PLUGIN_NAME) == 0)
            && plugin_id)
        {
            ptr_server = irc_server_search (plugin_id);
            if (ptr_server)
            {
                converted_addr[0] = '\0';
                local_address = weechat_infolist_string (infolist,
                                                         "local_address");
                if (local_address)
                {
                    res_init ();
                    rc = getaddrinfo (local_address, NULL, NULL, &ainfo);
                    if ((rc == 0) && ainfo && ainfo->ai_addr)
                    {
                        if (ainfo->ai_family == AF_INET)
                        {
                            /* transform dotted 4 IP address to ulong string */
                            saddr = (struct sockaddr_in *)ainfo->ai_addr;
                            snprintf (converted_addr, sizeof (converted_addr),
                                      "%lu",
                                      (unsigned long)ntohl (saddr->sin_addr.s_addr));
                        }
                        else
                        {
                            snprintf (converted_addr, sizeof (converted_addr),
                                      "%s", local_address);
                        }
                    }
                }

                type = weechat_infolist_string (infolist, "type_string");
                if (type && converted_addr[0])
                {
                    /* send DCC PRIVMSG */
                    if (strcmp (type, "file_send") == 0)
                    {
                        filename = weechat_infolist_string (infolist, "filename");
                        spaces_in_name = (strchr (filename, ' ') != NULL);
                        irc_server_sendf (
                            ptr_server,
                            IRC_SERVER_SEND_OUTQ_PRIO_HIGH, NULL,
                            "PRIVMSG %s :\01DCC SEND %s%s%s "
                            "%s %d %s\01",
                            weechat_infolist_string (infolist, "remote_nick"),
                            (spaces_in_name) ? "\"" : "",
                            filename,
                            (spaces_in_name) ? "\"" : "",
                            converted_addr,
                            weechat_infolist_integer (infolist, "port"),
                            weechat_infolist_string (infolist, "size"));
                    }
                    else if (strcmp (type, "chat_send") == 0)
                    {
                        irc_server_sendf (
                            ptr_server,
                            IRC_SERVER_SEND_OUTQ_PRIO_HIGH, NULL,
                            "PRIVMSG %s :\01DCC CHAT chat %s %d\01",
                            weechat_infolist_string (infolist, "remote_nick"),
                            converted_addr,
                            weechat_infolist_integer (infolist, "port"));
                    }
                }
            }
        }
    }

    weechat_infolist_reset_item_cursor (infolist);

    return WEECHAT_RC_OK;
}