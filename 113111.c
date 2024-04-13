ca_x509_subjectaltname_cmp(X509 *cert, struct iked_static_id *id)
{
	return ca_x509_subjectaltname_do(cert, MODE_ALT_CMP, NULL, id, NULL);
}