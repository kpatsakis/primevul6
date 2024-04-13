TypedValue HHVM_FUNCTION(range,
                         const Variant& low,
                         const Variant& high,
                         const Variant& step /* = 1 */) {
  bool is_step_double = false;
  double dstep = 1.0;
  if (step.isDouble()) {
    dstep = step.toDouble();
    is_step_double = true;
  } else if (step.isString()) {
    int64_t sn;
    double sd;
    DataType stype = step.toString().get()->isNumericWithVal(sn, sd, 0);
    if (stype == KindOfDouble) {
      is_step_double = true;
      dstep = sd;
    } else if (stype == KindOfInt64) {
      dstep = (double)sn;
    } else {
      dstep = step.toDouble();
    }
  } else {
    dstep = step.toDouble();
  }
  /* We only want positive step values. */
  if (dstep < 0.0) dstep *= -1;
  if (low.isString() && high.isString()) {
    String slow = low.toString();
    String shigh = high.toString();
    if (slow.size() >= 1 && shigh.size() >=1) {
      int64_t n1, n2;
      double d1, d2;
      DataType type1 = slow.get()->isNumericWithVal(n1, d1, 0);
      DataType type2 = shigh.get()->isNumericWithVal(n2, d2, 0);
      if (type1 == KindOfDouble || type2 == KindOfDouble || is_step_double) {
        if (type1 != KindOfDouble) d1 = slow.toDouble();
        if (type2 != KindOfDouble) d2 = shigh.toDouble();
        return tvReturn(ArrayUtil::Range(d1, d2, dstep));
      }

      int64_t lstep = (int64_t) dstep;
      if (type1 == KindOfInt64 || type2 == KindOfInt64) {
        if (type1 != KindOfInt64) n1 = slow.toInt64();
        if (type2 != KindOfInt64) n2 = shigh.toInt64();
        return tvReturn(ArrayUtil::Range((double)n1, (double)n2, lstep));
      }

      return tvReturn(ArrayUtil::Range((unsigned char)slow.charAt(0),
                                       (unsigned char)shigh.charAt(0), lstep));
    }
  }

  if (low.is(KindOfDouble) || high.is(KindOfDouble) || is_step_double) {
    return tvReturn(ArrayUtil::Range(low.toDouble(), high.toDouble(), dstep));
  }

  int64_t lstep = (int64_t) dstep;
  return tvReturn(ArrayUtil::Range(low.toDouble(), high.toDouble(), lstep));
}