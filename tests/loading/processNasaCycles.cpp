#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <matio.h>
#include <string>

#include <Eigen/Dense>
#include "Loading/ProcessNasaCycles.h"


using ::testing::Exactly;
using ::testing::DoubleEq;
using ::testing::_;

class MockCycleHandler : public CycleHandler {
public:
    MOCK_METHOD(void, onCharge, (const Eigen::VectorXd &voltage,
                             const Eigen::VectorXd &current,
                             const Eigen::VectorXd &temperature,
                             const Eigen::VectorXd &time), (override));

    MOCK_METHOD(void, onDischarge, (const Eigen::VectorXd &voltage,
                                    const Eigen::VectorXd &current,
                                    const Eigen::VectorXd &temperature,
                                    const Eigen::VectorXd &time,
                                    const double &capacity), (override));

    MOCK_METHOD(void, onImpedance, (double Z, double Re), (override));
};

TEST(ProcessNasaCyclesTest, InvokesCorrectHandlerCalls) {
    const std::string fileName = "/mnt/c/Users/Danie/Desktop/project/data/B0007.mat";
    const std::string varName = "B0007";


    MockCycleHandler mock;
    EXPECT_CALL(mock, onCharge(_, _, _, _)).Times(Exactly(170));
    // Expect one onDischarge
    EXPECT_CALL(mock, onDischarge(_, _, _, _, _)).Times(Exactly(168));

    EXPECT_CALL(mock, onImpedance(_, _)).Times(Exactly(278));

    //Call function under test
    ProcessNasaCycles(fileName, varName, mock);
}