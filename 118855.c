irc_nick_print_log (struct t_irc_nick *nick)
{
    weechat_log_printf ("");
    weechat_log_printf ("    => nick %s (addr:0x%lx):",    nick->name, nick);
    weechat_log_printf ("         host . . . . . : '%s'",  nick->host);
    weechat_log_printf ("         prefixes . . . : '%s'",  nick->prefixes);
    weechat_log_printf ("         prefix . . . . : '%s'",  nick->prefix);
    weechat_log_printf ("         away . . . . . : %d",    nick->away);
    weechat_log_printf ("         account. . . . : '%s'",  nick->account);
    weechat_log_printf ("         realname . . . : '%s'",  nick->realname);
    weechat_log_printf ("         color. . . . . : '%s'",  nick->color);
    weechat_log_printf ("         prev_nick. . . : 0x%lx", nick->prev_nick);
    weechat_log_printf ("         next_nick. . . : 0x%lx", nick->next_nick);
}