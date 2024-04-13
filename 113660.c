bool HHVM_FUNCTION(mcrypt_module_close, const Resource& td) {
  td.getTyped<MCrypt>()->close();
  return true;
}