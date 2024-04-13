DocumentSourceGroup::rewriteGroupAsTransformOnFirstDocument() const {
    if (_idExpressions.size() != 1) {
        // This transformation is only intended for $group stages that group on a single field.
        return nullptr;
    }

    auto fieldPathExpr = dynamic_cast<ExpressionFieldPath*>(_idExpressions.front().get());
    if (!fieldPathExpr || fieldPathExpr->isVariableReference()) {
        return nullptr;
    }

    const auto fieldPath = fieldPathExpr->getFieldPath();
    if (fieldPath.getPathLength() == 1) {
        // The path is $$CURRENT or $$ROOT. This isn't really a sensible value to group by (since
        // each document has a unique _id, it will just return the entire collection). We only
        // apply the rewrite when grouping by a single field, so we cannot apply it in this case,
        // where we are grouping by the entire document.
        invariant(fieldPath.getFieldName(0) == "CURRENT" || fieldPath.getFieldName(0) == "ROOT");
        return nullptr;
    }

    const auto groupId = fieldPath.tail().fullPath();

    // We can't do this transformation if there are any non-$first accumulators.
    for (auto&& accumulator : _accumulatedFields) {
        if (AccumulatorDocumentsNeeded::kFirstDocument !=
            accumulator.makeAccumulator()->documentsNeeded()) {
            return nullptr;
        }
    }

    std::vector<std::pair<std::string, boost::intrusive_ptr<Expression>>> fields;

    boost::intrusive_ptr<Expression> idField;
    // The _id field can be specified either as a fieldpath (ex. _id: "$a") or as a singleton
    // object (ex. _id: {v: "$a"}).
    if (_idFieldNames.empty()) {
        idField = ExpressionFieldPath::deprecatedCreate(pExpCtx.get(), groupId);
    } else {
        invariant(_idFieldNames.size() == 1);
        idField = ExpressionObject::create(pExpCtx.get(),
                                           {{_idFieldNames.front(), _idExpressions.front()}});
    }
    fields.push_back(std::make_pair("_id", idField));

    for (auto&& accumulator : _accumulatedFields) {
        fields.push_back(std::make_pair(accumulator.fieldName, accumulator.expr.argument));

        // Since we don't attempt this transformation for non-$first accumulators,
        // the initializer should always be trivial.
    }

    return GroupFromFirstDocumentTransformation::create(pExpCtx, groupId, std::move(fields));
}