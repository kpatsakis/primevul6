static void sxe_properties_add(Array& rv, char* name, const Variant& value) {
  String sName = String(name);
  if (rv.exists(sName)) {
    Variant existVal = rv[sName];
    if (existVal.isArray()) {
      Array arr = existVal.toArray();
      arr.append(value);
      rv.set(sName, arr);
    } else {
      Array arr = Array::Create();
      arr.append(existVal);
      arr.append(value);
      rv.set(sName, arr);
    }
  } else {
    rv.set(sName, value);
  }
}