#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <Eigen/Dense>

// Constants
const double MIN_MU = 0.0, MAX_MU = 10.0;
const double MIN_SIGMA = 0.01, MAX_SIGMA = 10.0;
const double MIN_RHO = 0.0, MAX_RHO = 0.99;
const double MIN_MIX_PARAM = 0.01, MAX_MIX_PARAM = 0.99;

struct Theta {
    double mu, sigma, rho, p;
    Eigen::Vector4d toVector() const { return Eigen::Vector4d(mu, sigma, rho, p); }
};

// Function declarations
double NormalCDFInverse(double p);
Eigen::VectorXd compute_pseudo_values(const Eigen::VectorXd& ranks, double mu, double sigma, double p);
Eigen::VectorXd calc_post_membership_prbs(const Theta& t, const Eigen::VectorXd& z1, const Eigen::VectorXd& z2);

#endif