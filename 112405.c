int64_t HHVM_FUNCTION(count,
                      const Variant& var,
                      int64_t mode /* = 0 */) {
  switch (var.getType()) {
    case KindOfUninit:
    case KindOfNull:
      return 0;

    case KindOfBoolean:
    case KindOfInt64:
    case KindOfDouble:
    case KindOfPersistentString:
    case KindOfString:
    case KindOfResource:
      return 1;

    case KindOfPersistentArray:
    case KindOfArray:
      if ((CountMode)mode == CountMode::RECURSIVE) {
        const Array& arr_var = var.toCArrRef();
        return php_count_recursive(arr_var);
      }
      return var.getArrayData()->size();

    case KindOfObject:
      {
        Object obj = var.toObject();
        if (obj->isCollection()) {
          return collections::getSize(obj.get());
        }
        if (obj.instanceof(SystemLib::s_CountableClass)) {
          return obj->o_invoke_few_args(s_count, 0).toInt64();
        }
      }
      return 1;

    case KindOfRef:
    case KindOfClass:
      break;
  }
  not_reached();
}