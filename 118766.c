irc_server_casesearch (const char *server_name)
{
    struct t_irc_server *ptr_server;

    if (!server_name)
        return NULL;

    for (ptr_server = irc_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        if (weechat_strcasecmp (ptr_server->name, server_name) == 0)
            return ptr_server;
    }

    /* server not found */
    return NULL;
}