static inline bool checkSetHelper(const req::ptr<c_Set>& st,
                                  const Cell c,
                                  TypedValue* strTv,
                                  bool convertIntLikeStrs) {
  if (c.m_type == KindOfInt64) {
    return st->contains(c.m_data.num);
  }
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
    return st->contains(n);
  }
  return st->contains(s);
}