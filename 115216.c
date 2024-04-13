TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  OpContext op_context(context, node);

  // When the 'size_splits' and the 'axis' tensor is non-const we can't resize
  // output tensors in Prepare(), and we have to do it now.
  if (!IsConstantTensor(op_context.axis) ||
      !IsConstantTensor(op_context.size_splits)) {
    TF_LITE_ENSURE_OK(
        context, ResizeOutputTensors(context, node, op_context.input,
                                     op_context.size_splits, op_context.axis));
  }

  int axis_value = GetTensorData<int>(op_context.axis)[0];

  // Use split function to build the outputs since they share the same logic.
#define TF_LITE_SPLIT_V(scalar)                                     \
  VectorOfTensors<scalar> all_outputs(*context, *node->outputs);    \
  tflite::SplitParams op_params;                                    \
  op_params.num_split = NumOutputs(node);                           \
  op_params.axis = axis_value;                                      \
  reference_ops::Split(op_params, GetTensorShape(op_context.input), \
                       GetTensorData<scalar>(op_context.input),     \
                       all_outputs.shapes(), all_outputs.data());
  switch (op_context.input->type) {
    case kTfLiteFloat32: {
      TF_LITE_SPLIT_V(float);
      break;
    }
    case kTfLiteUInt8: {
      TF_LITE_SPLIT_V(uint8_t);
      break;
    }
    case kTfLiteInt16: {
      TF_LITE_SPLIT_V(int16_t);
      break;
    }
    case kTfLiteInt32: {
      TF_LITE_SPLIT_V(int32_t);
      break;
    }
    case kTfLiteInt64: {
      TF_LITE_SPLIT_V(int64_t);
      break;
    }
    case kTfLiteInt8: {
      TF_LITE_SPLIT_V(int8_t);
      break;
    }
    default:
      context->ReportError(context, "Type %s currently not supported.",
                           TfLiteTypeGetName(op_context.input->type));
      return kTfLiteError;
  }
#undef TF_LITE_SPLIT_V

  return kTfLiteOk;
}