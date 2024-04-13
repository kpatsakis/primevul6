Array HHVM_FUNCTION(hash_algos) {
  Array ret;
  for (HashEngineMap::const_iterator iter = HashEngines.begin();
       iter != HashEngines.end(); ++iter) {
    ret.append(String(iter->first));
  }
  return ret;
}