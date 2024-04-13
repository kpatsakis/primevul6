boost::optional<DocumentSource::DistributedPlanLogic> DocumentSourceGroup::distributedPlanLogic() {
    intrusive_ptr<DocumentSourceGroup> mergingGroup(new DocumentSourceGroup(pExpCtx));
    mergingGroup->setDoingMerge(true);

    VariablesParseState vps = pExpCtx->variablesParseState;
    /* the merger will use the same grouping key */
    mergingGroup->setIdExpression(ExpressionFieldPath::parse(pExpCtx.get(), "$$ROOT._id", vps));

    for (auto&& accumulatedField : _accumulatedFields) {
        // The merger's output field names will be the same, as will the accumulator factories.
        // However, for some accumulators, the expression to be accumulated will be different. The
        // original accumulator may be collecting an expression based on a field expression or
        // constant.  Here, we accumulate the output of the same name from the prior group.
        auto copiedAccumulatedField = accumulatedField;
        copiedAccumulatedField.expr.argument = ExpressionFieldPath::parse(
            pExpCtx.get(), "$$ROOT." + copiedAccumulatedField.fieldName, vps);
        mergingGroup->addAccumulator(copiedAccumulatedField);
        mergingGroup->_memoryTracker.set(copiedAccumulatedField.fieldName, 0);
    }

    // {shardsStage, mergingStage, sortPattern}
    return DistributedPlanLogic{this, mergingGroup, boost::none};
}