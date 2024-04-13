Variant HHVM_FUNCTION(imageaffinematrixconcat,
                      const Array& m1,
                      const Array& m2) {
  int nelem1 = m1.size();
  int nelem2 = m2.size();
  int i;
  double dm1[6];
  double dm2[6];
  double dmr[6];
  Array ret = Array::Create();

  if (nelem1 != 6 || nelem2 != 6) {
    raise_warning("imageaffinematrixconcat(): Affine array must "
                  "have six elements");
    return false;
  }

  for (i = 0; i < 6; i++) {
    if (m1[i].isInteger()) {
      dm1[i] = m1[i].toInt64();
    } else if (m1[i].isDouble() || m1[i].isString()) {
      dm1[i] = m1[i].toDouble();
    } else {
      raise_warning("imageaffinematrixconcat(): Invalid type for "
                    "element %i", i);
      return false;
    }
    if (m2[i].isInteger()) {
      dm2[i] = m2[i].toInt64();
    } else if (m2[i].isDouble() || m2[i].isString()) {
      dm2[i] = m2[i].toDouble();
    } else {
      raise_warning("imageaffinematrixconcat():Invalid type for"
                    "element %i", i);
      return false;
    }
  }
  if (gdAffineConcat(dmr, dm1, dm2) != GD_TRUE) {
    return false;
  }

  for (i = 0; i < 6; i++) {
    ret.set(String(i, CopyString), dmr[i]);
  }
  return ret;
}