int64_t HHVM_FUNCTION(mcrypt_enc_get_iv_size, const Resource& td) {
  return mcrypt_enc_get_iv_size(td.getTyped<MCrypt>()->m_td);
}