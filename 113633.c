static int64_t HHVM_METHOD(SimpleXMLElement, count) {
  auto data = Native::data<SimpleXMLElement>(this_);
  return php_sxe_count_elements_helper(data);
}