isdn_ppp_get_slot(void)
{
	int i;
	for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
		if (!ippp_table[i]->state)
			return i;
	}
	return -1;
}