void TFE_Py_Execute(TFE_Context* ctx, const char* device_name,
                    const char* op_name, TFE_InputTensorHandles* inputs,
                    PyObject* attrs, TFE_OutputTensorHandles* outputs,
                    TF_Status* out_status) {
  TFE_Py_ExecuteCancelable(ctx, device_name, op_name, inputs, attrs,
                           /*cancellation_manager=*/nullptr, outputs,
                           out_status);
}