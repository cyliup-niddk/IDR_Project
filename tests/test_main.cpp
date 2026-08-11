/**
 * Unit Tests for IDR Analysis
 * Tests core functionality of utility and optimization modules
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <Eigen/Dense>
#include "utility.hpp"
#include "optimization.hpp"

// Test utilities
#define TEST(name) \
    std::cout << "Testing: " << name << "... "; \
    try {

#define ASSERT_TRUE(condition) \
    assert(condition);

#define ASSERT_NEAR(a, b, tol) \
    assert(std::abs((a) - (b)) < tol);

#define END_TEST \
    std::cout << "✓ PASSED" << std::endl; \
    } catch(const std::exception& e) { \
        std::cout << "✗ FAILED: " << e.what() << std::endl; \
    }

// Test counter
int tests_passed = 0;
int tests_failed = 0;

void test_theta_structure() {
    TEST("Theta structure creation")
    {
        Theta t = {1.5, 0.8, 0.6, 0.7};
        ASSERT_TRUE(t.mu == 1.5);
        ASSERT_TRUE(t.sigma == 0.8);
        ASSERT_TRUE(t.rho == 0.6);
        ASSERT_TRUE(t.p == 0.7);
        tests_passed++;
    }
    END_TEST
}

void test_theta_to_vector() {
    TEST("Theta to vector conversion")
    {
        Theta t = {1.0, 0.5, 0.6, 0.7};
        Eigen::Vector4d vec = t.toVector();
        ASSERT_NEAR(vec[0], 1.0, 1e-6);
        ASSERT_NEAR(vec[1], 0.5, 1e-6);
        ASSERT_NEAR(vec[2], 0.6, 1e-6);
        ASSERT_NEAR(vec[3], 0.7, 1e-6);
        tests_passed++;
    }
    END_TEST
}

void test_normal_cdf_inverse() {
    TEST("Normal CDF inverse - boundary")
    {
        double val_0 = NormalCDFInverse(0.0);
        double val_1 = NormalCDFInverse(1.0);
        ASSERT_TRUE(val_0 <= -5.0);
        ASSERT_TRUE(val_1 >= 5.0);
        tests_passed++;
    }
    END_TEST

    TEST("Normal CDF inverse - median")
    {
        double val_med = NormalCDFInverse(0.5);
        ASSERT_NEAR(val_med, 0.0, 0.1);
        tests_passed++;
    }
    END_TEST

    TEST("Normal CDF inverse - monotonicity")
    {
        double v1 = NormalCDFInverse(0.25);
        double v2 = NormalCDFInverse(0.50);
        double v3 = NormalCDFInverse(0.75);
        ASSERT_TRUE(v1 < v2 && v2 < v3);
        tests_passed++;
    }
    END_TEST
}

void test_compute_pseudo_values() {
    TEST("Pseudo-value computation")
    {
        Eigen::VectorXd ranks = Eigen::VectorXd::LinSpaced(10, 0, 9);
        Eigen::VectorXd z = compute_pseudo_values(ranks, 1.0, 1.0, 0.5);
        
        ASSERT_TRUE(z.size() == 10);
        for(int i = 0; i < z.size(); ++i) {
            ASSERT_TRUE(std::isfinite(z[i]));
        }
        tests_passed++;
    }
    END_TEST
}

void test_parameters_constrained() {
    TEST("Parameter constraints in EM step")
    {
        Eigen::VectorXd z1 = Eigen::VectorXd::LinSpaced(50, 0, 1);
        Eigen::VectorXd z2 = Eigen::VectorXd::LinSpaced(50, 0, 1);
        Theta current = {1.0, 0.5, 0.5, 0.5};
        
        Theta updated = EM_step(z1, z2, current);
        
        // Check constraints
        ASSERT_TRUE(updated.mu >= MIN_MU && updated.mu <= MAX_MU);
        ASSERT_TRUE(updated.sigma >= MIN_SIGMA && updated.sigma <= MAX_SIGMA);
        ASSERT_TRUE(updated.rho >= MIN_RHO && updated.rho <= MAX_RHO);
        ASSERT_TRUE(updated.p >= MIN_MIX_PARAM && updated.p <= MAX_MIX_PARAM);
        tests_passed++;
    }
    END_TEST
}

void test_idr_convergence() {
    TEST("IDR optimization convergence")
    {
        Eigen::VectorXd r1 = Eigen::VectorXd::LinSpaced(100, 1, 100);
        Eigen::VectorXd r2 = Eigen::VectorXd::LinSpaced(100, 1, 100);
        Theta initial = {1.0, 1.0, 0.5, 0.5};
        
        Theta result = run_idr(r1, r2, initial);
        
        // Check valid parameters
        ASSERT_TRUE(std::isfinite(result.mu));
        ASSERT_TRUE(std::isfinite(result.sigma));
        ASSERT_TRUE(std::isfinite(result.rho));
        ASSERT_TRUE(std::isfinite(result.p));
        tests_passed++;
    }
    END_TEST
}

int main() {
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "IDR Analysis - C++ Unit Tests" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << std::endl;

    // Run all tests
    test_theta_structure();
    test_theta_to_vector();
    test_normal_cdf_inverse();
    test_compute_pseudo_values();
    test_parameters_constrained();
    test_idr_convergence();

    // Summary
    std::cout << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Test Results: " << tests_passed << " passed";
    if(tests_failed > 0) {
        std::cout << ", " << tests_failed << " failed";
    }
    std::cout << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    return tests_failed == 0 ? 0 : 1;
}
