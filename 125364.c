Document DocumentSourceGroup::makeDocument(const Value& id,
                                           const Accumulators& accums,
                                           bool mergeableOutput) {
    const size_t n = _accumulatedFields.size();
    MutableDocument out(1 + n);

    /* add the _id field */
    out.addField("_id", expandId(id));

    /* add the rest of the fields */
    for (size_t i = 0; i < n; ++i) {
        Value val = accums[i]->getValue(mergeableOutput);
        if (val.missing()) {
            // we return null in this case so return objects are predictable
            out.addField(_accumulatedFields[i].fieldName, Value(BSONNULL));
        } else {
            out.addField(_accumulatedFields[i].fieldName, std::move(val));
        }
    }

    _stats.totalOutputDataSizeBytes += out.getApproximateSize();
    return out.freeze();
}