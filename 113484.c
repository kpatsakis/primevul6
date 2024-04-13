Variant HHVM_FUNCTION(mcrypt_enc_get_supported_key_sizes, const Resource& td) {
  auto pm = get_valid_mcrypt_resource(td);
  if (!pm) {
    return false;
  }

  int count = 0;
  int *key_sizes =
    mcrypt_enc_get_supported_key_sizes(pm->m_td, &count);

  Array ret = Array::Create();
  for (int i = 0; i < count; i++) {
    ret.append(key_sizes[i]);
  }
  mcrypt_free(key_sizes);
  return ret;
}