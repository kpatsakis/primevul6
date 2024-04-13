void TFE_Py_ExecuteCancelable(TFE_Context* ctx, const char* device_name,
                              const char* op_name,
                              TFE_InputTensorHandles* inputs, PyObject* attrs,
                              TFE_CancellationManager* cancellation_manager,
                              TFE_OutputTensorHandles* outputs,
                              TF_Status* out_status) {
  tensorflow::profiler::TraceMe activity(
      "TFE_Py_ExecuteCancelable", tensorflow::profiler::TraceMeLevel::kInfo);

  TFE_Op* op = GetOp(ctx, op_name, device_name, out_status);

  auto cleaner = tensorflow::gtl::MakeCleanup([ctx, op] { ReturnOp(ctx, op); });
  if (!out_status->status.ok()) return;

  tensorflow::unwrap(op)->SetStackTrace(tensorflow::GetStackTrace(
      tensorflow::StackTrace::kStackTraceInitialSize));

  for (int i = 0; i < inputs->size() && out_status->status.ok(); ++i) {
    TFE_OpAddInput(op, inputs->at(i), out_status);
  }
  if (cancellation_manager && out_status->status.ok()) {
    TFE_OpSetCancellationManager(op, cancellation_manager, out_status);
  }
  if (out_status->status.ok()) {
    SetOpAttrs(ctx, op, attrs, 0, out_status);
  }
  Py_BEGIN_ALLOW_THREADS;

  int num_outputs = outputs->size();

  if (out_status->status.ok()) {
    TFE_Execute(op, outputs->data(), &num_outputs, out_status);
  }

  if (out_status->status.ok()) {
    outputs->resize(num_outputs);
  } else {
    TF_SetStatus(out_status, TF_GetCode(out_status),
                 tensorflow::strings::StrCat(TF_Message(out_status),
                                             " [Op:", op_name, "]")
                     .c_str());
  }

  Py_END_ALLOW_THREADS;
}