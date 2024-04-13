static void HHVM_METHOD(SimpleXMLElement, __construct,
                        const String& data,
                        int64_t options /* = 0 */,
                        bool data_is_url /* = false */,
                        const String& ns /* = "" */,
                        bool is_prefix /* = false */) {
  SYNC_VM_REGS_SCOPED();
  xmlDocPtr docp = data_is_url ?
    xmlReadFile(data.data(), nullptr, options) :
    xmlReadMemory(data.data(), data.size(), nullptr, nullptr, options);
  if (!docp) {
    SystemLib::throwExceptionObject("String could not be parsed as XML");
  }

  auto sxe = Native::data<SimpleXMLElement>(this_);
  sxe->iter.nsprefix = !ns.empty() ? xmlStrdup((xmlChar*)ns.data()) : nullptr;
  sxe->iter.isprefix = is_prefix;
  sxe->node = libxml_register_node(xmlDocGetRootElement(docp));
}