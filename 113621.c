  void moduleInit() override {
    HHVM_FE(simplexml_import_dom);
    HHVM_FE(simplexml_load_string);
    HHVM_FE(simplexml_load_file);

    /* SimpleXMLElement */
    HHVM_ME(SimpleXMLElement, __construct);
    HHVM_ME(SimpleXMLElement, xpath);
    HHVM_ME(SimpleXMLElement, registerXPathNamespace);
    HHVM_ME(SimpleXMLElement, asXML);
    HHVM_ME(SimpleXMLElement, getNamespaces);
    HHVM_ME(SimpleXMLElement, getDocNamespaces);
    HHVM_ME(SimpleXMLElement, children);
    HHVM_ME(SimpleXMLElement, getName);
    HHVM_ME(SimpleXMLElement, attributes);
    HHVM_ME(SimpleXMLElement, addChild);
    HHVM_ME(SimpleXMLElement, addAttribute);
    HHVM_ME(SimpleXMLElement, __toString);
    HHVM_ME(SimpleXMLElement, __get);
    HHVM_ME(SimpleXMLElement, __unset);
    HHVM_ME(SimpleXMLElement, __isset);
    HHVM_ME(SimpleXMLElement, __set);
    HHVM_ME(SimpleXMLElement, count);
    HHVM_ME(SimpleXMLElement, offsetExists);
    HHVM_ME(SimpleXMLElement, offsetGet);
    HHVM_ME(SimpleXMLElement, offsetSet);
    HHVM_ME(SimpleXMLElement, offsetUnset);

    Native::registerNativeDataInfo<SimpleXMLElement>(
      s_SimpleXMLElement.get()
    );

    /* SimpleXMLElementIterator */
    HHVM_ME(SimpleXMLElementIterator, __construct);
    HHVM_ME(SimpleXMLElementIterator, current);
    HHVM_ME(SimpleXMLElementIterator, key);
    HHVM_ME(SimpleXMLElementIterator, next);
    HHVM_ME(SimpleXMLElementIterator, rewind);
    HHVM_ME(SimpleXMLElementIterator, valid);

    Native::registerNativeDataInfo<SimpleXMLElementIterator>(
      s_SimpleXMLElementIterator.get(),
      Native::NDIFlags::NO_SWEEP
    );

    /* SimpleXMLIterator */
    HHVM_ME(SimpleXMLIterator, current);
    HHVM_ME(SimpleXMLIterator, key);
    HHVM_ME(SimpleXMLIterator, next);
    HHVM_ME(SimpleXMLIterator, rewind);
    HHVM_ME(SimpleXMLIterator, valid);
    HHVM_ME(SimpleXMLIterator, getChildren);
    HHVM_ME(SimpleXMLIterator, hasChildren);

    Native::registerNativeDataInfo<SimpleXMLIterator>(
      s_SimpleXMLIterator.get()
    );

    loadSystemlib();
  }