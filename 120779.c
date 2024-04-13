static int idprime_match_card(sc_card_t *card)
{
	int i, r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	i = _sc_match_atr(card, idprime_atrs, &card->type);
	if (i < 0)
		return 0;

	r = idprime_select_index(card);
	return (r > 0);
}