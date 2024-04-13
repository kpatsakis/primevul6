static Variant HHVM_FUNCTION(simplexml_load_string,
  const String& data,
  const String& class_name /* = "SimpleXMLElement" */,
  int64_t options /* = 0 */,
  const String& ns /* = "" */,
  bool is_prefix /* = false */) {
  SYNC_VM_REGS_SCOPED();
  auto cls = class_from_name(class_name, "simplexml_load_string");
  if (!cls) {
    return init_null();
  }

  xmlDocPtr doc = xmlReadMemory(data.data(), data.size(), nullptr,
                                     nullptr, options);
  if (!doc) {
    return false;
  }

  Object obj = create_object(cls->nameStr(), Array(), false);
  auto sxe = Native::data<SimpleXMLElement>(obj.get());
  sxe->node = libxml_register_node(xmlDocGetRootElement(doc));
  sxe->iter.nsprefix = ns.size() ? xmlStrdup((xmlChar*)ns.data()) : nullptr;
  sxe->iter.isprefix = is_prefix;
  return obj;
}