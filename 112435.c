TypedValue HHVM_FUNCTION(array_sum,
                         const Variant& input) {
  if (UNLIKELY(!isContainer(input))) {
    raise_warning("Invalid operand type was used: %s expects "
                  "an array or collection as argument 1",
                  __FUNCTION__+2);
    return make_tv<KindOfNull>();
  }

  int64_t i = 0;
  ArrayIter iter(input);
  for (; iter; ++iter) {
    const Variant& entry(iter.secondRefPlus());

    switch (entry.getType()) {
      case KindOfUninit:
      case KindOfNull:
      case KindOfBoolean:
      case KindOfInt64:
      case KindOfRef:
        i += entry.toInt64();
        continue;

      case KindOfDouble:
        goto DOUBLE;

      case KindOfPersistentString:
      case KindOfString: {
        int64_t ti;
        double td;
        if (entry.getStringData()->isNumericWithVal(ti, td, 1) ==
            KindOfInt64) {
          i += ti;
          continue;
        } else {
          goto DOUBLE;
        }
      }

      case KindOfPersistentArray:
      case KindOfArray:
      case KindOfObject:
      case KindOfResource:
        continue;

      case KindOfClass:
        break;
    }
    not_reached();
  }
  return make_tv<KindOfInt64>(i);

DOUBLE:
  double d = i;
  for (; iter; ++iter) {
    const Variant& entry(iter.secondRefPlus());
    switch (entry.getType()) {
      DT_UNCOUNTED_CASE:
      case KindOfString:
      case KindOfRef:
        d += entry.toDouble();

      case KindOfArray:
      case KindOfObject:
      case KindOfResource:
        continue;

      case KindOfClass:
        break;
    }
    not_reached();
  }
  return make_tv<KindOfDouble>(d);
}