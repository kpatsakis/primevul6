  Status CalculateMultiplesFromChain(const std::vector<const NodeDef*>& chain,
                                     Tensor* multiples) {
    // Keep track of how the multiples correspond to each shape dimension.
    // For example, given Stack([x, x], axis=1) with rank(x) = 3, we start with
    //    multiples=[1, 1, 1] , dims=[0, 1, 2]
    // After processing the stack op
    //    multiples=[1, 2, 1] , dims=[0, 1, 1, 2]
    std::vector<int32> dims(multiples->NumElements());
    std::iota(dims.begin(), dims.end(), 0);

    for (int i = 0; i < multiples->NumElements(); ++i) {
      multiples->flat<int32>()(i) = 1;
    }

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
      AttrSlice attrs(**it);
      int64 axis, n;
      TF_RETURN_IF_ERROR(GetNodeAttr(attrs, "axis", &axis));
      TF_RETURN_IF_ERROR(GetNodeAttr(attrs, "N", &n));

      if (axis >= dims.size()) {
        // We don't handle the case where Pack is performed on the last axis,
        // e.g. Pack([x, x], axis=3) where rank(x) == 3
        return Status(error::OUT_OF_RANGE, "axis value out of range of dims");
      }

      int64 m = multiples->flat<int32>()(dims[axis]) * n;
      if (TF_PREDICT_FALSE(m > INT_MAX)) {
        return Status(error::OUT_OF_RANGE, "int32 overflow");
      }
      multiples->flat<int32>()(dims[axis]) = static_cast<int32>(m);

      // Copy index from immediate right of inserted axis
      dims.insert(dims.begin() + axis, dims[axis]);
    }

    return Status::OK();
  }