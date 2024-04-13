debug_certificate_name(const char *msg, X509_NAME *name)
{
  BIO *bio;

  if (name == nullptr) {
    return;
  }

  bio = BIO_new(BIO_s_mem());
  if (bio == nullptr) {
    return;
  }

  if (X509_NAME_print_ex(bio, name, 0 /* indent */, XN_FLAG_ONELINE) > 0) {
    long len;
    char *ptr;
    len = BIO_get_mem_data(bio, &ptr);
    Debug("ssl", "%s %.*s", msg, (int)len, ptr);
  }

  BIO_free(bio);
}