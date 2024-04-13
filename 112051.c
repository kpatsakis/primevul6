  explicit DequantizeOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
    string mode_string;
    OP_REQUIRES_OK(ctx, ctx->GetAttr("mode", &mode_string));
    OP_REQUIRES(
        ctx,
        (ctx->output_type(0) == DT_FLOAT || ctx->output_type(0) == DT_BFLOAT16),
        errors::InvalidArgument("Output type must be bfloat16 or float,"
                                " is '" +
                                DataTypeString(ctx->output_type(0)) + "'"));

    need_cast_ = true;
    if (ctx->output_type(0) == DT_FLOAT) {
      need_cast_ = false;
      OP_REQUIRES(ctx,
                  (mode_string == "MIN_COMBINED" ||
                   mode_string == "MIN_FIRST" || mode_string == "SCALED"),
                  errors::InvalidArgument("Mode string must be 'MIN_COMBINED',"
                                          " 'MIN_FIRST', or 'SCALED', is '" +
                                          mode_string + "'"));
    } else {
      OP_REQUIRES(
          ctx, (mode_string == "MIN_COMBINED"),
          errors::InvalidArgument("When output type is bfloat16, Mode"
                                  " string must be 'MIN_COMBINED', is '" +
                                  mode_string + "'"));
    }

    if (mode_string == "MIN_COMBINED") {
      mode_ = QUANTIZE_MODE_MIN_COMBINED;
    } else if (mode_string == "MIN_FIRST") {
      mode_ = QUANTIZE_MODE_MIN_FIRST;
    } else if (mode_string == "SCALED") {
      mode_ = QUANTIZE_MODE_SCALED;
    }
    OP_REQUIRES_OK(ctx, ctx->GetAttr("narrow_range", &narrow_range_));
    OP_REQUIRES_OK(ctx, ctx->GetAttr("axis", &axis_));
  }