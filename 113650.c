String HHVM_FUNCTION(mcrypt_enc_get_algorithms_name, const Resource& td) {
  char *name = mcrypt_enc_get_algorithms_name(td.getTyped<MCrypt>()->m_td);
  String ret(name, CopyString);
  mcrypt_free(name);
  return ret;
}