ca_sslerror(const char *caller)
{
	unsigned long	 error;

	while ((error = ERR_get_error()) != 0)
		log_warnx("%s: %s: %.100s", __func__, caller,
		    ERR_error_string(error, NULL));
}