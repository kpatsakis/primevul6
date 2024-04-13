Variant SimpleXMLElement_objectCast(const ObjectData* obj, int8_t type) {
  assert(obj->instanceof(SimpleXMLElement_classof()));
  auto sxe = Native::data<SimpleXMLElement>(const_cast<ObjectData*>(obj));
  if (type == KindOfBoolean) {
    xmlNodePtr node = php_sxe_get_first_node(sxe, nullptr);
    if (node) return true;
    Array properties = Array::Create();
    sxe_get_prop_hash(sxe, true, properties, true);
    return properties.size() != 0;
  }
  if (isArrayType((DataType)type)) {
    Array properties = Array::Create();
    sxe_get_prop_hash(sxe, true, properties);
    return properties;
  }

  xmlChar* contents = nullptr;
  if (sxe->iter.type != SXE_ITER_NONE) {
    xmlNodePtr node = php_sxe_get_first_node(sxe, nullptr);
    if (node) {
      contents = xmlNodeListGetString(sxe->docp(), node->children, 1);
    }
  } else {
    xmlDocPtr doc = sxe->docp();
    if (!sxe->nodep()) {
      if (doc) {
        sxe->node = libxml_register_node(xmlDocGetRootElement(doc));
      }
    }

    if (sxe->nodep()) {
      if (sxe->nodep()->children) {
        contents = xmlNodeListGetString(doc, sxe->nodep()->children, 1);
      }
    }
  }

  String ret = String((char*)contents);
  if (contents) {
    xmlFree(contents);
  }

  switch (type) {
    case KindOfString: return ret;
    case KindOfInt64:  return toInt64(ret);
    case KindOfDouble: return toDouble(ret);
    default:           return init_null();
  }
}