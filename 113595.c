static inline void sxe_add_namespace_name(Array& ret, xmlNsPtr ns) {
  String prefix = String(SXE_NS_PREFIX(ns));
  if (!ret.exists(prefix)) {
    ret.set(prefix, String((char*)ns->href, CopyString));
  }
}