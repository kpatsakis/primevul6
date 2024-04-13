bool HHVM_FUNCTION(natcasesort, VRefParam array) {
  return php_asort(array, SORT_NATURAL_CASE, true, false);
}