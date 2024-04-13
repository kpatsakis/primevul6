bool HHVM_FUNCTION(asort,
                   VRefParam array,
                   int sort_flags /* = 0 */) {
  bool use_zend_sort = RuntimeOption::EnableZendSorting;
  return php_asort(array, sort_flags, true, use_zend_sort);
}