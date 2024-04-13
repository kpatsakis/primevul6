Array HHVM_FUNCTION(mcrypt_module_get_supported_key_sizes,
                    const String& algorithm,
                    const String& lib_dir /* = null_string */) {
  String dir = lib_dir.empty() ? String(MCG(algorithms_dir)) : lib_dir;

  int count = 0;
  int *key_sizes = mcrypt_module_get_algo_supported_key_sizes
    ((char*)algorithm.data(), (char*)dir.data(), &count);

  Array ret = Array::Create();
  for (int i = 0; i < count; i++) {
    ret.append(key_sizes[i]);
  }
  mcrypt_free(key_sizes);
  return ret;
}