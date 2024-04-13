ca_sslinit(void)
{
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	/* Init hardware crypto engines. */
	ENGINE_load_builtin_engines();
	ENGINE_register_all_complete();
}