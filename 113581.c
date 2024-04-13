  void setSxe(const Object& sxe) {
    assert(sxe->instanceof(SimpleXMLElement_classof()));
    m_sxe = Object(sxe.get());
  }