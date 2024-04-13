  SegmentSumOpModel(const TensorData& data, const TensorData& segment_ids) {
    data_id_ = AddInput(data);
    segment_ids_id_ = AddInput(segment_ids);
    output_id_ = AddOutput(data.type);
    SetBuiltinOp(BuiltinOperator_SEGMENT_SUM, BuiltinOptions_NONE, 0);
    BuildInterpreter({GetShape(data_id_), GetShape(segment_ids_id_)});
  }