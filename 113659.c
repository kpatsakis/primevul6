int64_t HHVM_FUNCTION(mcrypt_enc_get_block_size, const Resource& td) {
  return mcrypt_enc_get_block_size(td.getTyped<MCrypt>()->m_td);
}