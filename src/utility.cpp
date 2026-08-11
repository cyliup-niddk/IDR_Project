#include "utility.hpp"
#include <cmath>

// Rational approximation for inverse normal CDF (Wichura, 1988)
// Uses Hastings approximation with different variable arrangement
double RationalApproximation(double t) {
    double numerator_coeffs[] = {2.515517, 0.802853, 0.010328};
    double denominator_coeffs[] = {1.432788, 0.189269, 0.001308};
    
    // Build numerator: a0 + a1*t + a2*t^2
    double num = numerator_coeffs[0];
    num += numerator_coeffs[1] * t;
    num += numerator_coeffs[2] * t * t;
    
    // Build denominator: 1 + b1*t + b2*t^2 + b3*t^3
    double denom = 1.0;
    denom += denominator_coeffs[0] * t;
    denom += denominator_coeffs[1] * t * t;
    denom += denominator_coeffs[2] * t * t * t;
    
    return t - (num / denom);
}

// Inverse of standard normal cumulative distribution function
// Handles extreme values and uses rational approximation for main range
double NormalCDFInverse(double p) {
    // Boundary conditions for extreme probabilities
    if (p <= 0.0) {
        return -5.0;  // Lower bound approximation
    }
    if (p >= 1.0) {
        return 5.0;   // Upper bound approximation
    }
    
    // For lower half: use symmetry
    if (p < 0.5) {
        double lower_tail = std::sqrt(-2.0 * std::log(p));
        return -1.0 * RationalApproximation(lower_tail);
    }
    
    // For upper half: direct approximation
    double upper_tail = std::sqrt(-2.0 * std::log(1.0 - p));
    return RationalApproximation(upper_tail);
}

// Transform ranks to normal quantiles (pseudo-values)
// Maps empirical ranks to standard normal distribution
Eigen::VectorXd compute_pseudo_values(const Eigen::VectorXd& ranks, double mu, double sigma, double p) {
    (void)mu; (void)sigma; (void)p;  // Parameters not used in quantile transformation
    
    int vector_size = ranks.size();
    Eigen::VectorXd quantiles(vector_size);
    double normalizing_factor = static_cast<double>(vector_size) + 1.0;
    
    // Convert each rank to normalized position in (0, 1)
    // Then apply inverse normal CDF
    for (int idx = 0; idx < vector_size; ++idx) {
        double normalized_rank = (ranks[idx] + 1.0) / normalizing_factor;
        quantiles[idx] = NormalCDFInverse(normalized_rank);
    }
    
    return quantiles;
}

// Calculate posterior probability of membership in signal component
// Uses bivariate normal mixture model
Eigen::VectorXd calc_post_membership_prbs(const Theta& t, const Eigen::VectorXd& z1, const Eigen::VectorXd& z2) {
    int num_samples = z1.size();
    Eigen::VectorXd posterior_probs(num_samples);
    
    // Precompute bivariate normal covariance determinant
    double correlation_sq = t.rho * t.rho;
    double covariance_det = 1.0 - correlation_sq;
    
    // Normalization constant for signal component
    double signal_normalizer = 1.0 / (2.0 * M_PI * t.sigma * std::sqrt(covariance_det));
    
    // Normalization constant for noise component (uncorrelated bivariate standard normal)
    double noise_normalizer = 1.0 / (2.0 * M_PI);
    
    // For each observation, compute likelihood ratio
    for (int j = 0; j < num_samples; ++j) {
        double z1_centered = z1[j] - t.mu;
        double z2_centered = z2[j] - t.mu;
        
        // Quadratic form for bivariate normal: (z - mu)^T Sigma^-1 (z - mu)
        double cross_term = z1_centered * z2_centered;
        double quad_form = (z1_centered * z1_centered + z2_centered * z2_centered - 
                           2.0 * t.rho * cross_term) / (covariance_det * t.sigma * t.sigma);
        
        // Signal component: bivariate normal with correlation
        double signal_likelihood = signal_normalizer * std::exp(-0.5 * quad_form);
        
        // Noise component: independent standard normals
        double noise_likelihood = noise_normalizer * std::exp(-0.5 * (z1[j] * z1[j] + z2[j] * z2[j]));
        
        // Posterior probability using mixture model
        double signal_weighted = t.p * signal_likelihood;
        double noise_weighted = (1.0 - t.p) * noise_likelihood;
        posterior_probs[j] = signal_weighted / (signal_weighted + noise_weighted);
    }
    
    return posterior_probs;
}