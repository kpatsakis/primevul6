ca_asn1_name(uint8_t *asn1, size_t len)
{
	X509_NAME	*name = NULL;
	char		*str = NULL;
	const uint8_t	*p;

	p = asn1;
	if ((name = d2i_X509_NAME(NULL, &p, len)) == NULL)
		return (NULL);
	str = X509_NAME_oneline(name, NULL, 0);
	X509_NAME_free(name);

	return (str);
}