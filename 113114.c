ca_x509_subject_cmp(X509 *cert, struct iked_static_id *id)
{
	X509_NAME	*subject, *idname = NULL;
	const uint8_t	*idptr;
	size_t		 idlen;
	int		 ret = -1;

	if (id->id_type != IKEV2_ID_ASN1_DN)
		return (-1);
	if ((subject = X509_get_subject_name(cert)) == NULL)
		return (-1);
	if (id->id_length <= id->id_offset)
		return (-1);
	idlen = id->id_length - id->id_offset;
	idptr = id->id_data + id->id_offset;
	if ((idname = d2i_X509_NAME(NULL, &idptr, idlen)) == NULL)
		return (-1);
	if (X509_NAME_cmp(subject, idname) == 0)
		ret = 0;
	X509_NAME_free(idname);
	return (ret);
}