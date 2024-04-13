sc_pkcs15emu_coolkey_init_ex(sc_pkcs15_card_t *p15card,
		struct sc_aid *aid)
{
	sc_card_t      *card = p15card->card;
	sc_context_t    *ctx = card->ctx;
	int rv;

	LOG_FUNC_CALLED(ctx);

	rv = coolkey_detect_card(p15card);
	if (rv)
		LOG_FUNC_RETURN(ctx, SC_ERROR_WRONG_CARD);
	rv = sc_pkcs15emu_coolkey_init(p15card);

	LOG_FUNC_RETURN(ctx, rv);
}