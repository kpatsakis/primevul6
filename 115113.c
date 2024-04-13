  Status Init(DataType input_type, const string& fill = "", int width = -1,
              int precision = -1, bool scientific = false,
              bool shortest = false) {
    TF_CHECK_OK(NodeDefBuilder("op", "AsString")
                    .Input(FakeInput(input_type))
                    .Attr("fill", fill)
                    .Attr("precision", precision)
                    .Attr("scientific", scientific)
                    .Attr("shortest", shortest)
                    .Attr("width", width)
                    .Finalize(node_def()));
    return InitOp();
  }