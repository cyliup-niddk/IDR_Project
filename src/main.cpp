#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <numeric>
#include "optimization.hpp"

// Structure to match the 'Peak' namedtuple in idr.py
struct Peak {
    std::string chrom;
    int start;
    int stop;
    double signal;
};

// Function to load narrowPeak files (Column 7 is signalValue)
std::vector<Peak> load_narrow_peak(const std::string& filename) {
    std::vector<Peak> peaks;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string chrom, dummy;
        int start, stop;
        double signal;
        
        // narrowPeak format: chrom, start, stop, name, score, strand, signalValue...
        ss >> chrom >> start >> stop >> dummy >> dummy >> dummy >> signal;
        peaks.push_back({chrom, start, stop, signal});
    }
    return peaks;
}

// Basic overlap merger (Simplified version of merge_peaks in idr.py)
void merge_and_rank(const std::vector<Peak>& p1, const std::vector<Peak>& p2, 
                    Eigen::VectorXd& r1, Eigen::VectorXd& r2) {
    std::vector<double> s1_vals, s2_vals;

    // For every peak in replicate 1, find an overlapping peak in replicate 2
    for (const auto& a : p1) {
        for (const auto& b : p2) {
            if (a.chrom == b.chrom && std::max(a.start, b.start) < std::min(a.stop, b.stop)) {
                s1_vals.push_back(a.signal);
                s2_vals.push_back(b.signal);
                break; 
            }
        }
    }

    // Convert signals to ranks to match build_rank_vectors in idr.py
    int n = s1_vals.size();
    r1.resize(n);
    r2.resize(n);

    auto get_ranks = [](std::vector<double> scores) {
        std::vector<int> indices(scores.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(), [&](int i, int j) {
            return scores[i] < scores[j];
        });
        Eigen::VectorXd ranks(scores.size());
        for (size_t i = 0; i < indices.size(); ++i) ranks[indices[i]] = static_cast<double>(i);
        return ranks;
    };

    r1 = get_ranks(s1_vals);
    r2 = get_ranks(s2_vals);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <peak1> <peak2>" << std::endl;
        return 1;
    }

    // 1. Load the narrowPeak files
    auto peaks1 = load_narrow_peak(argv[1]);
    auto peaks2 = load_narrow_peak(argv[2]);

    // 2. Perform the overlap merge and ranking
    Eigen::VectorXd r1, r2;
    merge_and_rank(peaks1, peaks2, r1, r2);

    if (r1.size() < 20) {
        std::cerr << "Error: Fewer than 20 overlapping peaks found." << std::endl;
        return 1;
    }

    std::cout << "Processing " << r1.size() << " overlapping peaks..." << std::endl;

    // 3. Run the IDR optimization loop
    Theta initial = {1.0, 1.0, 0.5, 0.5}; // Default starting points
    Theta result = run_idr(r1, r2, initial);

    // 4. Output results
    std::cout << "\n--- Final IDR Model Parameters ---" << std::endl;
    std::cout << "Mu (Signal Mean):  " << result.mu << std::endl;
    std::cout << "Sigma (Std Dev):   " << result.sigma << std::endl;
    std::cout << "Rho (Correlation): " << result.rho << std::endl;
    std::cout << "P (Mix Proportion):" << result.p << std::endl;

    return 0;
}


// # 1. Enter the inner 'idr' directory where the code is
// cd ~/project/idr/idr

// # 2. Create the build folder here
// mkdir -p build
// cd build

// # 3. Now run cmake (pointing to the folder we just left)
// cmake ..

// # 4. Compile
// make