void ReadAlignment(OpKernelConstruction* context,
                   bool* left_align_superdiagonal,
                   bool* left_align_subdiagonal) {
  string align;
  OP_REQUIRES_OK(context, context->GetAttr("align", &align));

  *left_align_superdiagonal = align == "LEFT_LEFT" || align == "LEFT_RIGHT";
  *left_align_subdiagonal = align == "LEFT_LEFT" || align == "RIGHT_LEFT";
}