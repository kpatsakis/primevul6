  ~ArraySortTmp() {
    if (m_ad != m_arr.get()) {
      m_arr = Array::attach(m_ad);
    }
  }