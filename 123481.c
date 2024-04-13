TEST(SegmentSumOpModelTest, Float32Test_ThreeDimensions) {
  SegmentSumOpModel<float> model({TensorType_FLOAT32, {3, 2, 1}},
                                 {TensorType_INT32, {3}});
  model.PopulateTensor<float>(model.data(), {1, 2, 3, 4, 5, 6});
  model.PopulateTensor<int32_t>(model.segment_ids(), {0, 0, 1});
  model.Invoke();
  EXPECT_THAT(model.GetOutput(), ElementsAreArray({4.0f, 6.0f, 5.0f, 6.0f}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({2, 2, 1}));
}