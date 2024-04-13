bool HHVM_FUNCTION(hash_update, const Resource& context, const String& data) {
  HashContext *hash = context.getTyped<HashContext>();
  hash->ops->hash_update(hash->context, (unsigned char *)data.data(),
                         data.size());
  return true;
}