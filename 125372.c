intrusive_ptr<DocumentSource> DocumentSourceGroup::createFromBson(
    BSONElement elem, const intrusive_ptr<ExpressionContext>& expCtx) {
    uassert(15947, "a group's fields must be specified in an object", elem.type() == Object);

    intrusive_ptr<DocumentSourceGroup> groupStage(new DocumentSourceGroup(expCtx));

    BSONObj groupObj(elem.Obj());
    BSONObjIterator groupIterator(groupObj);
    VariablesParseState vps = expCtx->variablesParseState;
    while (groupIterator.more()) {
        BSONElement groupField(groupIterator.next());
        StringData pFieldName = groupField.fieldNameStringData();
        if (pFieldName == "_id") {
            uassert(15948,
                    "a group's _id may only be specified once",
                    groupStage->_idExpressions.empty());
            groupStage->setIdExpression(parseIdExpression(expCtx, groupField, vps));
            invariant(!groupStage->_idExpressions.empty());
        } else if (pFieldName == "$doingMerge") {
            massert(17030, "$doingMerge should be true if present", groupField.Bool());

            groupStage->setDoingMerge(true);
        } else {
            // Any other field will be treated as an accumulator specification.
            groupStage->addAccumulator(
                AccumulationStatement::parseAccumulationStatement(expCtx.get(), groupField, vps));
            groupStage->_memoryTracker.set(pFieldName, 0);
        }
    }

    uassert(
        15955, "a group specification must include an _id", !groupStage->_idExpressions.empty());
    return groupStage;
}