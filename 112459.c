bool HHVM_FUNCTION(natsort, VRefParam array) {
  return php_asort(array, SORT_NATURAL, true, false);
}