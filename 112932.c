static HashEnginePtr php_hash_fetch_ops(const String& algo) {
  HashEngineMap::const_iterator iter =
    HashEngines.find(f_strtolower(algo).data());
  if (iter == HashEngines.end()) {
    return HashEnginePtr();
  }
  return iter->second;
}