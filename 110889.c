NAN_METHOD(TxnWrap::putString) {
    return putCommon(info, [](Nan::NAN_METHOD_ARGS_TYPE info, MDB_val &data) -> void {
        CustomExternalStringResource::writeTo(Local<String>::Cast(info[2]), &data);
    }, [](MDB_val &data) -> void {
        delete[] (uint16_t*)data.mv_data;
    });
}