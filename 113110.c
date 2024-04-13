ca_cert_info(const char *msg, X509 *cert)
{
	ASN1_INTEGER	*asn1_serial;
	BUF_MEM		*memptr;
	BIO		*rawserial = NULL;
	char		 buf[BUFSIZ];

	if ((asn1_serial = X509_get_serialNumber(cert)) == NULL ||
	    (rawserial = BIO_new(BIO_s_mem())) == NULL ||
	    i2a_ASN1_INTEGER(rawserial, asn1_serial) <= 0)
		goto out;
	if (X509_NAME_oneline(X509_get_issuer_name(cert), buf, sizeof(buf)))
		log_info("%s: issuer: %s", msg, buf);
	BIO_get_mem_ptr(rawserial, &memptr);
	if (memptr->data != NULL && memptr->length < INT32_MAX)
		log_info("%s: serial: %.*s", msg, (int)memptr->length,
		    memptr->data);
	if (X509_NAME_oneline(X509_get_subject_name(cert), buf, sizeof(buf)))
		log_info("%s: subject: %s", msg, buf);
	ca_x509_subjectaltname_log(cert, msg);
out:
	if (rawserial)
		BIO_free(rawserial);
}