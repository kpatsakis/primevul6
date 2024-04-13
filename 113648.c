int64_t HHVM_FUNCTION(mcrypt_enc_self_test, const Resource& td) {
  return mcrypt_enc_self_test(td.getTyped<MCrypt>()->m_td);
}