ca_x509_subjectaltname_get(X509 *cert, struct iked_id *retid)
{
	return ca_x509_subjectaltname_do(cert, MODE_ALT_GET, NULL, NULL, retid);
}