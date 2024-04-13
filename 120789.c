static int idprime_select_idprime(sc_card_t *card)
{
	return iso_ops->select_file(card, &idprime_path, NULL);
}