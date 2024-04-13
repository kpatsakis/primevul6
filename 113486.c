Array HHVM_FUNCTION(mcrypt_list_algorithms,
                    const String& lib_dir /* = null_string */) {
  String dir = lib_dir.empty() ? String(MCG(algorithms_dir)) : lib_dir;

  int count = 0;
  char **modules = mcrypt_list_algorithms((char*)dir.data(), &count);
  if (count == 0) {
    raise_warning("No algorithms found in module dir");
  }
  Array ret = Array::Create();
  for (int i = 0; i < count; i++) {
    ret.append(String(modules[i], CopyString));
  }
  mcrypt_free_p(modules, count);
  return ret;
}