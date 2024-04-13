ca_x509_subjectaltname_log(X509 *cert, const char *logmsg)
{
	return ca_x509_subjectaltname_do(cert, MODE_ALT_LOG, logmsg, NULL, NULL);
}