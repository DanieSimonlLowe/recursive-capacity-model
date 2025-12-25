#include "OcvEstimation/OcvEstimator.h"

template<typename RLS>
class Rls1EcmOcvEstimator: public OcvEstimator {

    public:
        Rls1EcmOcvEstimator(const Eigen::VectorXd& params);

        void update(const Eigen::VectorXd& current, 
                        const Eigen::VectorXd& voltage) override;
        void setDeltaTime(double deltaTime) override;

        double getOcv() override;
        bool canCalculateState() override;

        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();

    private:
        RLS rls;
};

#include "OcvEstimation/Rls1EcmOcvEstimator.tpp"