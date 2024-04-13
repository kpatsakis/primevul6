static Variant HHVM_FUNCTION(simplexml_load_file,
                              const String& filename,
                              const String& class_name /* = "SimpleXMLElement" */,
                              int64_t options /* = 0 */, const String& ns /* = "" */,
                              bool is_prefix /* = false */) {
  SYNC_VM_REGS_SCOPED();
  auto cls = class_from_name(class_name, "simplexml_load_file");
  if (!cls) {
    return init_null();
  }

  auto stream = File::Open(filename, "rb");
  if (!stream || stream->isInvalid()) return false;

  xmlDocPtr doc = nullptr;

  // The XML context is also deleted in this function, so the ownership
  // of the File is kept locally in 'stream'. The libxml_streams_IO_nop_close
  // callback does nothing.
  xmlParserCtxtPtr ctxt = xmlCreateIOParserCtxt(nullptr, nullptr,
                                                libxml_streams_IO_read,
                                                libxml_streams_IO_nop_close,
                                                &stream,
                                                XML_CHAR_ENCODING_NONE);
  if (ctxt == nullptr) return false;
  SCOPE_EXIT { xmlFreeParserCtxt(ctxt); };

  if (ctxt->directory == nullptr) {
    ctxt->directory = xmlParserGetDirectory(filename.c_str());
  }
  xmlParseDocument(ctxt);
  if (ctxt->wellFormed) {
    doc = ctxt->myDoc;
  } else {
    xmlFreeDoc(ctxt->myDoc);
    ctxt->myDoc = nullptr;
    return false;
  }

  Object obj = create_object(cls->nameStr(), Array(), false);
  auto sxe = Native::data<SimpleXMLElement>(obj.get());
  sxe->node = libxml_register_node(xmlDocGetRootElement(doc));
  sxe->iter.nsprefix = ns.size() ? xmlStrdup((xmlChar*)ns.data()) : nullptr;
  sxe->iter.isprefix = is_prefix;
  return obj;
}