static Variant HHVM_METHOD(SimpleXMLElement, asXML,
                           const String& filename /* = "" */) {
  auto data = Native::data<SimpleXMLElement>(this_);
  xmlNodePtr node = data->nodep();
  xmlOutputBufferPtr outbuf = nullptr;

  if (filename.size()) {
    node = php_sxe_get_first_node(data, node);

    if (node) {
      xmlDocPtr doc = data->docp();
      if (node->parent && (XML_DOCUMENT_NODE == node->parent->type)) {
        int bytes;
        bytes = xmlSaveFile(filename.data(), doc);
        if (bytes == -1) {
          return false;
        } else {
          return true;
        }
      } else {
        outbuf = xmlOutputBufferCreateFilename(filename.data(), nullptr, 0);

        if (outbuf == nullptr) {
          return false;
        }

        xmlNodeDumpOutput(outbuf, doc, node, 0, 0, nullptr);
        xmlOutputBufferClose(outbuf);
        return true;
      }
    } else {
      return false;
    }
  }

  node = php_sxe_get_first_node(data, node);

  if (node) {
    xmlDocPtr doc = data->docp();
    if (node->parent && (XML_DOCUMENT_NODE == node->parent->type)) {
      xmlChar* strval;
      int strval_len;
      xmlDocDumpMemoryEnc(doc, &strval, &strval_len,
                          (const char*)doc->encoding);
      String ret = String((char*)strval);
      xmlFree(strval);
      return ret;
    } else {
      /* Should we be passing encoding information instead of nullptr? */
      outbuf = xmlAllocOutputBuffer(nullptr);

      if (outbuf == nullptr) {
        return false;
      }

      xmlNodeDumpOutput(outbuf, doc, node, 0, 0,
                        (const char*)doc->encoding);
      xmlOutputBufferFlush(outbuf);

      char* str = nullptr;
#ifdef LIBXML2_NEW_BUFFER
      str = (char*)xmlOutputBufferGetContent(outbuf);
#else
      str = (char*)outbuf->buffer->content;
#endif
      String ret = String(str);
      xmlOutputBufferClose(outbuf);
      return ret;
    }
  } else {
    return false;
  }
  return false;
}