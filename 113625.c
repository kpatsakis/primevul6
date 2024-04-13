bool SimpleXMLElement_propEmpty(const ObjectData* this_,
                                const StringData* key) {
  auto data = Native::data<SimpleXMLElement>(const_cast<ObjectData*>(this_));
  return !sxe_prop_dim_exists(data, Variant(key->toCppString()),
                              true, true, false);
}