bool HHVM_FUNCTION(mcrypt_enc_is_block_algorithm, const Resource& td) {
  return mcrypt_enc_is_block_algorithm(td.getTyped<MCrypt>()->m_td) == 1;
}