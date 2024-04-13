static inline void updateIntersectMapHelper(const req::ptr<c_Map>& mp,
                                            const Cell c,
                                            int pos,
                                            TypedValue* strTv,
                                            bool convertIntLikeStrs) {
  if (c.m_type == KindOfInt64) {
    auto val = mp->get(c.m_data.num);
    if (val && val->m_data.num == pos) {
      assert(val->m_type == KindOfInt64);
      ++val->m_data.num;
    }
  } else {
    StringData* s;
    if (LIKELY(isStringType(c.m_type))) {
      s = c.m_data.pstr;
    } else {
      s = tvCastToString(&c);
      decRefStr(strTv->m_data.pstr);
      strTv->m_data.pstr = s;
    }
    int64_t n;
    if (convertIntLikeStrs && s->isStrictlyInteger(n)) {
      auto val = mp->get(n);
      if (val && val->m_data.num == pos) {
        assert(val->m_type == KindOfInt64);
        ++val->m_data.num;
      }
    } else {
      auto val = mp->get(s);
      if (val && val->m_data.num == pos) {
        assert(val->m_type == KindOfInt64);
        ++val->m_data.num;
      }
    }
  }
}