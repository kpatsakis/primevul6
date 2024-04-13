CellularNetwork::DataLeft CellularNetwork::data_left() const {
  if (data_plans_.empty())
    return DATA_NORMAL;
  const CellularDataPlan& plan(data_plans_[0]);
  if (plan.plan_type == CELLULAR_DATA_PLAN_UNLIMITED) {
    base::TimeDelta remaining = plan.plan_end_time - plan.update_time;
    if (remaining <= base::TimeDelta::FromSeconds(0))
      return DATA_NONE;
    else if (remaining <=
        base::TimeDelta::FromSeconds(kCellularDataVeryLowSecs))
      return DATA_VERY_LOW;
    else if (remaining <= base::TimeDelta::FromSeconds(kCellularDataLowSecs))
      return DATA_LOW;
    else
      return DATA_NORMAL;
  } else if (plan.plan_type == CELLULAR_DATA_PLAN_METERED_PAID ||
             plan.plan_type == CELLULAR_DATA_PLAN_METERED_BASE) {
    int64 remaining = plan.plan_data_bytes - plan.data_bytes_used;
    if (remaining <= 0)
      return DATA_NONE;
    else if (remaining <= kCellularDataVeryLowBytes)
      return DATA_VERY_LOW;
    else if (remaining <= kCellularDataLowBytes)
      return DATA_LOW;
    else
      return DATA_NORMAL;
  }
  return DATA_NORMAL;
}
