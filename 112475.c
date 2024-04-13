TypedValue HHVM_FUNCTION(array_unshift,
                         VRefParam array,
                         const Variant& var,
                         const Array& args /* = null array */) {
  const auto* cell_array = array->asCell();
  if (UNLIKELY(!isContainer(*cell_array))) {
    raise_warning("%s() expects parameter 1 to be an array, Vector, or Set",
                  __FUNCTION__+2 /* remove the "f_" prefix */);
    return make_tv<KindOfNull>();
  }
  if (isArrayType(cell_array->m_type)) {
    auto ref_array = array.getVariantOrNull();
    if (!ref_array) {
      return make_tv<KindOfInt64>(
        cell_array->m_data.parr->size() + args.size() + 1
      );
    }
    if (cell_array->m_data.parr->isVectorData()) {
      if (!args.empty()) {
        auto pos_limit = args->iter_end();
        for (ssize_t pos = args->iter_last(); pos != pos_limit;
             pos = args->iter_rewind(pos)) {
          ref_array->asArrRef().prepend(args->getValueRef(pos));
        }
      }
      ref_array->asArrRef().prepend(var);
    } else {
      {
        Array newArray;
        newArray.append(var);
        if (!args.empty()) {
          auto pos_limit = args->iter_end();
          for (ssize_t pos = args->iter_begin(); pos != pos_limit;
               pos = args->iter_advance(pos)) {
            newArray.append(args->getValueRef(pos));
          }
        }
        for (ArrayIter iter(array.toArray()); iter; ++iter) {
          Variant key(iter.first());
          const Variant& value(iter.secondRef());
          if (key.isInteger()) {
            newArray.appendWithRef(value);
          } else {
            newArray.setWithRef(key, value, true);
          }
        }
        *ref_array = std::move(newArray);
      }
      // Reset the array's internal pointer
      ref_array->asArrRef()->reset();
    }
    return make_tv<KindOfInt64>(ref_array->asArrRef().size());
  }
  // Handle collections
  assert(cell_array->m_type == KindOfObject);
  auto* obj = cell_array->m_data.pobj;
  assert(obj->isCollection());
  switch (obj->collectionType()) {
    case CollectionType::Vector: {
      auto* vec = static_cast<c_Vector*>(obj);
      if (!args.empty()) {
        auto pos_limit = args->iter_end();
        for (ssize_t pos = args->iter_last(); pos != pos_limit;
             pos = args->iter_rewind(pos)) {
          vec->addFront(args->getValueRef(pos).asCell());
        }
      }
      vec->addFront(var.asCell());
      return make_tv<KindOfInt64>(vec->size());
    }
    case CollectionType::Set: {
      auto* st = static_cast<c_Set*>(obj);
      if (!args.empty()) {
        auto pos_limit = args->iter_end();
        for (ssize_t pos = args->iter_last(); pos != pos_limit;
             pos = args->iter_rewind(pos)) {
          st->addFront(args->getValueRef(pos).asCell());
        }
      }
      st->addFront(var.asCell());
      return make_tv<KindOfInt64>(st->size());
    }
    case CollectionType::Map:
    case CollectionType::Pair:
    case CollectionType::ImmVector:
    case CollectionType::ImmMap:
    case CollectionType::ImmSet:
      break;
  }
  raise_warning("%s() expects parameter 1 to be an array, Vector, or Set",
                __FUNCTION__+2 /* remove the "f_" prefix */);
  return make_tv<KindOfNull>();
}