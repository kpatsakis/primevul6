  SimpleXMLElement* sxe() {
    assert(m_sxe->instanceof(SimpleXMLElement_classof()));
    return Native::data<SimpleXMLElement>(m_sxe.get());
  }