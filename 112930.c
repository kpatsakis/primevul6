Resource HHVM_FUNCTION(hash_copy, const Resource& context) {
  HashContext *oldhash = context.getTyped<HashContext>();
  auto const hash = new HashContext(oldhash);
  return Resource(hash);
}