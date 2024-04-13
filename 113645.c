bool HHVM_FUNCTION(mcrypt_enc_is_block_algorithm_mode, const Resource& td) {
  return mcrypt_enc_is_block_algorithm_mode(td.getTyped<MCrypt>()->m_td) == 1;
}