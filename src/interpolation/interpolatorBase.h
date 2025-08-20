#pragma once
#include <vector>
#include <stdexcept>
#include <Eigen/Dense>

class InterpolatorBase {
    
    public:
        virtual ~InterpolatorBase() = default;
        std::vector<double>  GetTimeSteps(double measurement, double time);
        virtual InterpolatorBase* clone() = 0;

    
        virtual double predict(double time) = 0;
        virtual void update(double measurement, double time) = 0;
        virtual bool canPredict() = 0;
        virtual void reset() = 0;
        virtual void setParams(Eigen::VectorXd& params); 
        virtual size_t getParamsCount();
        virtual Eigen::VectorXd getLowerBounds();
        virtual Eigen::VectorXd getUpperBounds();
    
    protected:
        double timeStepSize;

    private:
        
        double lastTime;
        double lastMeasuredTime;


        bool hasRun = false;
};


