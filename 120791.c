static struct sc_card_driver * sc_get_driver(void)
{
	if (iso_ops == NULL) {
		iso_ops = sc_get_iso7816_driver()->ops;
	}

	idprime_ops = *iso_ops;
	idprime_ops.match_card = idprime_match_card;
	idprime_ops.init = idprime_init;
	idprime_ops.finish = idprime_finish;

	idprime_ops.read_binary = idprime_read_binary;
	idprime_ops.select_file = idprime_select_file;
	idprime_ops.card_ctl = idprime_card_ctl;
	idprime_ops.set_security_env = idprime_set_security_env;
	idprime_ops.compute_signature = idprime_compute_signature;
	idprime_ops.decipher = idprime_decipher;

	return &idprime_drv;
}