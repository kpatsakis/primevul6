static void HHVM_METHOD(SimpleXMLElementIterator, __construct,
                        const Variant& sxe) {
  if (sxe.isObject()) {
    Native::data<SimpleXMLElementIterator>(this_)->setSxe(sxe.toObject());
  }
}