  explicit MatrixSetDiagOp(OpKernelConstruction* context) : OpKernel(context) {
    // MatrixSetDiagV3-specific.
    if (context->HasAttr("align")) {
      functor::ReadAlignment(context, &left_align_superdiagonal_,
                             &left_align_subdiagonal_);
    }
  }