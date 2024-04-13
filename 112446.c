static inline void addToIntersectMapHelper(const req::ptr<c_Map>& mp,
                                           const Cell c,
                                           TypedValue* intOneTv,
                                           TypedValue* strTv,
                                           bool convertIntLikeStrs) {
  if (c.m_type == KindOfInt64) {
    mp->set(c.m_data.num, intOneTv);
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
      mp->set(n, intOneTv);
    } else {
      mp->set(s, intOneTv);
    }
  }
}