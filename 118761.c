irc_server_auto_connect (int auto_connect)
{
    weechat_hook_timer (1, 0, 1,
                        &irc_server_auto_connect_timer_cb,
                        (auto_connect) ? (void *)1 : (void *)0,
                        NULL);
}