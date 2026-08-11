#include "optimization.hpp"
#include <algorithm>

// Expectation-Maximization step for IDR mixture model
// Updates parameters given current posterior membership probabilities
Theta EM_step(const Eigen::VectorXd& z1, const Eigen::VectorXd& z2, const Theta& current) {
    // E-step: compute posterior membership probabilities
    Eigen::VectorXd posterior = calc_post_membership_prbs(current, z1, z2);
    double total_posterior = posterior.sum();
    int data_size = z1.size();
    
    // M-step: update parameters
    // Update mean: weighted average of pooled data
    double new_mu = (posterior.array() * (z1.array() + z2.array())).sum() / (2.0 * total_posterior);
    
    // Compute deviations from new mean
    Eigen::VectorXd dev1 = z1.array() - new_mu;
    Eigen::VectorXd dev2 = z2.array() - new_mu;
    
    // Compute weighted sum of squared deviations
    double weighted_ss = (posterior.array() * (dev1.array().square() + dev2.array().square())).sum();
    
    // Update standard deviation: sqrt(wss / 2n')
    double new_sigma = std::sqrt(weighted_ss / (2.0 * total_posterior));
    
    // Update correlation: 2 * sum(w * dev1 * dev2) / wss
    double weighted_cross = (posterior.array() * dev1.array() * dev2.array()).sum();
    double new_rho = 2.0 * weighted_cross / weighted_ss;
    
    // Update mixture proportion: n'/n
    double new_p = total_posterior / data_size;
    
    // Apply constraints to ensure valid parameter space
    return Theta{
        std::clamp(new_mu, MIN_MU, MAX_MU),
        std::clamp(new_sigma, MIN_SIGMA, MAX_SIGMA),
        std::clamp(new_rho, MIN_RHO, MAX_RHO),
        std::clamp(new_p, MIN_MIX_PARAM, MAX_MIX_PARAM)
    };
}

// Execute EM algorithm for IDR parameter estimation
// Iterates until convergence or maximum iterations reached
Theta run_idr(const Eigen::VectorXd& r1, const Eigen::VectorXd& r2, Theta init) {
    Theta parameters = init;
    const int MAX_ITERATIONS = 100;
    const double CONVERGENCE_THRESHOLD = 1e-6;
    
    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration) {
        Theta previous_params = parameters;
        
        // Transform ranks to normal quantiles
        Eigen::VectorXd normal_scores_rep1 = compute_pseudo_values(r1, parameters.mu, parameters.sigma, parameters.p);
        Eigen::VectorXd normal_scores_rep2 = compute_pseudo_values(r2, parameters.mu, parameters.sigma, parameters.p);
        
        // Update parameters via EM step
        parameters = EM_step(normal_scores_rep1, normal_scores_rep2, parameters);
        
        // Check for convergence: L1 norm of parameter difference
        double parameter_change = (parameters.toVector() - previous_params.toVector()).lpNorm<1>();
        if (parameter_change < CONVERGENCE_THRESHOLD) {
            break;  // Algorithm converged
        }
    }
    
    return parameters;
}