  bool CanFuseAffineOp(Operation *affine_op, Operation *binary_op) const {
    if (!isa_and_nonnull<TFL::Conv2DOp, TFL::DepthwiseConv2DOp,
                         TFL::FullyConnectedOp>(affine_op)) {
      return false;
    }
    DenseElementsAttr value;
    // Check that bias are constants if not none.
    Value bias = affine_op->getOperand(2);
    if (!bias.getType().isa<NoneType>() &&
        !matchPattern(bias, m_Constant(&value))) {
      return false;
    }
    // If the binary op is mul/div, also check that filter is constant.
    if (isa<TFL::MulOp, TFL::DivOp>(binary_op) &&
        !matchPattern(affine_op->getOperand(1), m_Constant(&value))) {
      return false;
    }

    // We can only fuse F32/BF16.
    auto is_fusable_type = [](Type t) {
      Type element_type = t;
      if (auto shaped_type = t.dyn_cast<ShapedType>()) {
        element_type = shaped_type.getElementType();
      }
      return element_type.isBF16() || element_type.isF32();
    };
    for (Type t : binary_op->getOperandTypes()) {
      if (!is_fusable_type(t)) {
        return false;
      }
    }

    return true;
  }