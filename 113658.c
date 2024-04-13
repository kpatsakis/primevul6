Array HHVM_FUNCTION(mcrypt_enc_get_supported_key_sizes, const Resource& td) {
  int count = 0;
  int *key_sizes =
    mcrypt_enc_get_supported_key_sizes(td.getTyped<MCrypt>()->m_td, &count);

  Array ret = Array::Create();
  for (int i = 0; i < count; i++) {
    ret.append(key_sizes[i]);
  }
  mcrypt_free(key_sizes);
  return ret;
}