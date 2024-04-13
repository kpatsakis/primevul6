  explicit UnaryOpsComposition(const GraphOptimizerContext& ctx,
                               const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("UnaryOpsComposition", ctx, ctx_ext) {
    // WARN: This should be consistent with unary_ops_composition.cc.
    // clang-format off
    supported_ops_ = {// Ops defined via Eigen scalar ops.
                      {"Abs",        {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Acos",       {DT_FLOAT,          DT_DOUBLE}},
                      {"Acosh",      {DT_FLOAT,          DT_DOUBLE}},
                      {"Asin",       {DT_FLOAT,          DT_DOUBLE}},
                      {"Asinh",      {DT_FLOAT,          DT_DOUBLE}},
                      {"Atan",       {DT_FLOAT,          DT_DOUBLE}},
                      {"Atanh",      {DT_FLOAT,          DT_DOUBLE}},
                      {"Ceil",       {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Cos",        {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Cosh",       {DT_FLOAT,          DT_DOUBLE}},
                      {"Expm1",      {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Exp",        {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Floor",      {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Inv",        {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Log",        {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Log1p",      {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Neg",        {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Reciprocal", {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Rint",       {DT_FLOAT,          DT_DOUBLE}},
                      {"Round",      {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Rsqrt",      {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Sigmoid",    {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Sin",        {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Sinh",       {DT_FLOAT,          DT_DOUBLE}},
                      {"Sqrt",       {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Square",     {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Tan",        {DT_FLOAT,          DT_DOUBLE}},
                      {"Tanh",       {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      // Additional ops that are not part of the Eigen.
                      {"Elu",        {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Relu",       {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Relu6",      {DT_FLOAT, DT_HALF, DT_DOUBLE}},
                      {"Selu",       {DT_FLOAT, DT_HALF, DT_DOUBLE}}};
    // clang-format on
  }