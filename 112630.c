static String convertArg(const Variant& arg) {
  return arg.isNull() ? null_string : arg.toString();
}