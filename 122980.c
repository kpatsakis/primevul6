  explicit MatrixDiagPartOp(OpKernelConstruction* context) : OpKernel(context) {
    // MatrixDiagPartV3-specific.
    if (context->HasAttr("align")) {
      functor::ReadAlignment(context, &left_align_superdiagonal_,
                             &left_align_subdiagonal_);
    }
  }