print_tty_chars(TTY * old_settings, TTY * new_settings)
{
    show_tty_change(old_settings, new_settings, "Erase", VERASE, CERASE);
    show_tty_change(old_settings, new_settings, "Kill", VKILL, CKILL);
    show_tty_change(old_settings, new_settings, "Interrupt", VINTR, CINTR);
}