std::string FormatErrorStatusStackTrace(const tensorflow::Status& status) {
  tensorflow::DCheckPyGilState();
  DCHECK(!status.ok());

  if (status.stack_trace().empty()) return status.error_message();

  const std::vector<tensorflow::StackFrame>& stack_trace = status.stack_trace();

  PyObject* linecache = PyImport_ImportModule("linecache");
  PyObject* getline =
      PyObject_GetAttr(linecache, PyUnicode_FromString("getline"));
  DCHECK(getline);

  std::ostringstream result;
  result << "Exception originated from\n\n";

  for (const tensorflow::StackFrame& stack_frame : stack_trace) {
    PyObject* line_str_obj = PyObject_CallFunction(
        getline, const_cast<char*>("si"), stack_frame.file_name.c_str(),
        stack_frame.line_number);
    tensorflow::StringPiece line_str = TFE_GetPythonString(line_str_obj);
    tensorflow::str_util::RemoveWhitespaceContext(&line_str);
    result << "  File \"" << stack_frame.file_name << "\", line "
           << stack_frame.line_number << ", in " << stack_frame.function_name
           << '\n';

    if (!line_str.empty()) result << "    " << line_str << '\n';
    Py_XDECREF(line_str_obj);
  }

  Py_DecRef(getline);
  Py_DecRef(linecache);

  result << '\n' << status.error_message();
  return result.str();
}