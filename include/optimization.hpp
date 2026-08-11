#ifndef OPTIMIZATION_HPP
#define OPTIMIZATION_HPP

#include "utility.hpp"

Theta EM_step(const Eigen::VectorXd& z1, const Eigen::VectorXd& z2, const Theta& current);
Theta run_idr(const Eigen::VectorXd& r1, const Eigen::VectorXd& r2, Theta init);

#endif