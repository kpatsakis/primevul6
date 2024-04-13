static String memcache_prepare_key(const String& var) {
  String var_mutable(var, CopyString);
  auto data = var_mutable.get()->mutableData();
  for (int i = 0; i < var.length(); i++) {
    // This is a stupid encoding since it causes collisions but it matches php5
    if (data[i] <= ' ') {
      data[i] = '_';
    }
  }
  return data;
}