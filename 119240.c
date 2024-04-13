TEST(DenseOpticalFlow_VariationalRefinement, ReferenceAccuracy)
{
    Mat frame1, frame2, GT;
    ASSERT_TRUE(readRubberWhale(frame1, frame2, GT));
    float target_RMSE = 0.86f;
    cvtColor(frame1, frame1, COLOR_BGR2GRAY);
    cvtColor(frame2, frame2, COLOR_BGR2GRAY);

    Ptr<VariationalRefinement> var_ref;
    var_ref = VariationalRefinement::create();
    var_ref->setAlpha(20.0f);
    var_ref->setDelta(5.0f);
    var_ref->setGamma(10.0f);
    var_ref->setSorIterations(25);
    var_ref->setFixedPointIterations(25);
    Mat flow(frame1.size(), CV_32FC2);
    flow.setTo(0.0f);
    var_ref->calc(frame1, frame2, flow);
    ASSERT_EQ(GT.rows, flow.rows);
    ASSERT_EQ(GT.cols, flow.cols);
    EXPECT_LE(calcRMSE(GT, flow), target_RMSE);
}