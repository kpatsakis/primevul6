static inline void addToSetHelper(const req::ptr<c_Set>& st,
                                  const Cell c,
                                  TypedValue* strTv,
                                  bool convertIntLikeStrs) {
  if (c.m_type == KindOfInt64) {
    st->add(c.m_data.num);
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
      st->add(n);
    } else {
      st->add(s);
    }
  }
}