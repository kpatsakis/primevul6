irc_server_login (struct t_irc_server *server)
{
    const char *capabilities;
    char *password, *username, *realname, *username2;

    password = irc_server_eval_expression (
        server,
        IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_PASSWORD));
    username = irc_server_eval_expression (
        server,
        IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_USERNAME));
    realname = irc_server_eval_expression (
        server,
        IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_REALNAME));

    capabilities = IRC_SERVER_OPTION_STRING(
        server, IRC_SERVER_OPTION_CAPABILITIES);

    if (password && password[0])
    {
        irc_server_sendf (
            server, 0, NULL,
            "PASS %s%s",
            ((password[0] == ':') || (strchr (password, ' '))) ? ":" : "",
            password);
    }

    if (!server->nick)
    {
        irc_server_set_nick (server,
                             (server->nicks_array) ?
                             server->nicks_array[0] : "weechat");
        server->nick_first_tried = 0;
    }
    else
        server->nick_first_tried = irc_server_get_nick_index (server);

    server->nick_alternate_number = -1;

    if (irc_server_sasl_enabled (server) || (capabilities && capabilities[0]))
    {
        irc_server_sendf (server, 0, NULL, "CAP LS " IRC_SERVER_VERSION_CAP);
    }

    username2 = (username && username[0]) ?
        weechat_string_replace (username, " ", "_") : strdup ("weechat");
    irc_server_sendf (
        server, 0, NULL,
        "NICK %s%s\n"
        "USER %s 0 * :%s",
        (server->nick && strchr (server->nick, ':')) ? ":" : "",
        server->nick,
        (username2) ? username2 : "weechat",
        (realname && realname[0]) ? realname : ((username2) ? username2 : "weechat"));
    if (username2)
        free (username2);

    if (server->hook_timer_connection)
        weechat_unhook (server->hook_timer_connection);
    server->hook_timer_connection = weechat_hook_timer (
        IRC_SERVER_OPTION_INTEGER (server, IRC_SERVER_OPTION_CONNECTION_TIMEOUT) * 1000,
        0, 1,
        &irc_server_timer_connection_cb,
        server, NULL);

    if (password)
        free (password);
    if (username)
        free (username);
    if (realname)
        free (realname);
}