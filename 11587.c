set_control_chars(TTY * tty_settings, int my_erase, int my_intr, int my_kill)
{
    if (DISABLED(tty_settings->c_cc[VERASE]) || my_erase >= 0) {
	tty_settings->c_cc[VERASE] = UChar((my_erase >= 0)
					   ? my_erase
					   : default_erase());
    }

    if (DISABLED(tty_settings->c_cc[VINTR]) || my_intr >= 0) {
	tty_settings->c_cc[VINTR] = UChar((my_intr >= 0)
					  ? my_intr
					  : CINTR);
    }

    if (DISABLED(tty_settings->c_cc[VKILL]) || my_kill >= 0) {
	tty_settings->c_cc[VKILL] = UChar((my_kill >= 0)
					  ? my_kill
					  : CKILL);
    }
}