intrusive_ptr<DocumentSourceGroup> DocumentSourceGroup::create(
    const intrusive_ptr<ExpressionContext>& expCtx,
    const boost::intrusive_ptr<Expression>& groupByExpression,
    std::vector<AccumulationStatement> accumulationStatements,
    boost::optional<size_t> maxMemoryUsageBytes) {
    size_t memoryBytes = maxMemoryUsageBytes ? *maxMemoryUsageBytes
                                             : internalDocumentSourceGroupMaxMemoryBytes.load();
    intrusive_ptr<DocumentSourceGroup> groupStage(new DocumentSourceGroup(expCtx, memoryBytes));
    groupStage->setIdExpression(groupByExpression);
    for (auto&& statement : accumulationStatements) {
        groupStage->addAccumulator(statement);
        groupStage->_memoryTracker.set(statement.fieldName, 0);
    }

    return groupStage;
}