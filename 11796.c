to_left_margin(void)
{
    if (VALID_STRING(carriage_return)) {
	sent_string(carriage_return);
    } else {
	out_char('\r');
    }
    return TRUE;
}