bool HHVM_FUNCTION(ksort,
                   VRefParam array,
                   int sort_flags /* = 0 */) {
  bool use_zend_sort = RuntimeOption::EnableZendSorting;
  return php_ksort(array, sort_flags, true, use_zend_sort);
}