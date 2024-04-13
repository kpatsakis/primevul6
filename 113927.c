static int coolkey_detect_card(sc_pkcs15_card_t *p15card)
{
	sc_card_t *card = p15card->card;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	if (card->type < SC_CARD_TYPE_COOLKEY_GENERIC
		|| card->type >= SC_CARD_TYPE_COOLKEY_GENERIC+1000)
		return SC_ERROR_INVALID_CARD;
	return SC_SUCCESS;
}