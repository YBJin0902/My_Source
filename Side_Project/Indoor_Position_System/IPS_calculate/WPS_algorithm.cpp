#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <Eigen/Dense>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace Eigen;

vector<vector<double>> load_RSSI(int M_Candidate) 
{
	// 硂ㄧ计ノ弄CSV郎盢ㄤ锣传蝴疊翴计秖
	
    vector<vector<double>> output(M_Candidate, vector<double>(10));
    for (int i = 0; i < M_Candidate; ++i)
	{
        ifstream file("./ap" + to_string(i) + ",5GHz.csv");
        
        string line;
        
        while (getline(file, line)) 
		{
            stringstream ss(line);
            string cell;
            vector<double> row;
            
            while (getline(ss, cell, ',')) 
			{
                row.push_back(stod(cell));
            }
            output[i] = row;
        }
    }
    
    return output;
}

vector<vector<double>> normalize(vector<vector<double>>& data, double RSSI_MIN, double RSSI_MAX) 
{
	// 硂ㄧ计ノ盢计沮タ砏て
	
    for (auto& row : data) 
	{
        for (auto& cell : row) 
		{
            cell = (cell - RSSI_MIN) / (RSSI_MAX - RSSI_MIN);
        }
    }
    
    return data;
}

tuple<double, double, double,VectorXd, double, double> fitness_f(MatrixXd& data, int N_APS, int top_K, double RSSI_MIN, double w1, double w2, double w3, double w4) 
{
    // 盢计沮鹅N_APS︽痻皚
    data.resize(N_APS, data.size() / N_APS);

    // т–い程top_Kじま
    vector<int> topK_min(top_K);
    for (int i = 0; i < data.cols(); ++i) 
	{
        VectorXd col = data.col(i);
        partial_sort(col.data(), col.data() + top_K, col.data() + col.size(), greater<double>());
        
        for (int j = 0; j < top_K; ++j) 
		{
        	topK_min[j] = distance(data.col(i).data(), find(data.col(i).data(), data.col(i).data() + data.col(i).size(), col[j]));
        }
    }

    // 璸衡P
    double P = 0.0;
    
    for (int i = 0; i < data.cols(); ++i) 
	{
        int count = 0;
        
        for (int j = 0; j < top_K; ++j) 
		{
            if (data(topK_min[j], i) <= RSSI_MIN) 
			{
                ++count;
            }
        }
        
        P += static_cast<double>(count) / top_K;
    }
    
    P /= data.cols();

    // 盢计沮タ砏て
    double rssi_min = data.minCoeff();
    double rssi_max = data.maxCoeff();
    
    data = (data.array() - rssi_min) / (rssi_max - rssi_min);

    // 璸衡r_mean㎝r_variance
    double r_total = data.sum();
    
    double r_mean = r_total / (data.size() - (data.array().isNaN().cast<int>().sum()));
    
    double v_total = (data.array() - r_mean).pow(2).sum();
    
    double r_variance = v_total / (data.size() - (data.array().isNaN().cast<int>().sum()));

    // 璸衡鎖
    MatrixXd cov_matrix = (data.rowwise() - data.colwise().mean()).cov();
    
    Solver<MatrixXd> solver(cov_matrix);
    
    VectorXd P_eig_values = solver. values().real() / solver.values().real().sum();
    
    double entropy = -(P_eig_values.array() * P_eig_values.array().log() / log(2)).sum();

    // 璸衡J
    double J = w1 * 1 / (r_mean + 1e-8) + w2 * r_variance - w3 * entropy + exp(w4 * P);

    return make_tuple(J, r_mean, r_variance, P_eig_values, entropy, P);
}

// ﹚竡盽计㎝舦
const int M_Candidate = 132;  // 匡计秖
const int N_APS = 20;  // AP计秖
const double CROSS_RATE = 0.8;  // ユ瞯
const double MUTATE_RATE = 0.02;  // 跑瞯
const int POP_SIZE = 200;  // 竤砰
const int N_GENERATIONS = 600;  // 计秖
const double RSSI_MAX = -30;  // 程RSSI
const double RSSI_MIN = -80;  // 程RSSI

// ﹚竡舦
const double w1 = 0.8;  // r_mean舦
const double w2 = 0.2;  // r_variance舦
const double w3 = 10;  // S舦
const double w4 = 100;  // P舦
const int top_K = 20; 
const int cross_last_K = N_APS;

class GA 
{
public:
    GA(int DNA_size, double cross_rate, double mutation_rate, int pop_size)
        : DNA_size(DNA_size), cross_rate(cross_rate), mutate_rate(mutation_rate), pop_size(pop_size) 
	{
        candidate = vector<int>(M_Candidate);
        
        iota(candidate.begin(), candidate.end(), 0);
        
        pop = vector<vector<int>>(pop_size, vector<int>(N_APS));
        
        for (auto& p : pop) 
		{
            random_shuffle(candidate.begin(), candidate.end());
            copy(candidate.begin(), candidate.begin() + N_APS, p.begin());
            sort(p.begin(), p.end());
        }
        
        RSSI_MAP = load_RSSI(M_Candidate);
    }

    vector<int> translateDNA(const vector<int>& DNA) 
	{
        return DNA;
    }

    vector<double> get_fitness(const vector<vector<int>>& DNA) 
	{
        vector<double> fitness(DNA.size());
        
        for (int i = 0; i < DNA.size(); ++i) 
		{
            auto& AP = DNA[i];
            auto Configurations = RSSI_MAP[AP];
            fitness[i] = fitness_f(Configurations);
        }
        
        return fitness;
    }

    vector<vector<int>> select(const vector<int>& most_idx) 
	{
        vector<vector<int>> selected(10);
        
        for (auto& s : selected) 
		{
            s = pop[most_idx[rand() % most_idx.size()]];
        }
        
        return selected;
    }

    vector<int> crossover(const vector<int>& parent, const vector<vector<int>>& pop) 
	{
        if ((double)rand() / RAND_MAX < cross_rate) 
		{
            int i_ = rand() % cross_last_K;
            int j_ = rand() % cross_last_K;
            
            vector<int> all_DNA = parent;
            
            all_DNA.insert(all_DNA.end(), pop[j_].begin(), pop[j_].end());
            
            sort(all_DNA.begin(), all_DNA.end());
            
            all_DNA.erase(std::unique(all_DNA.begin(), all_DNA.end()), all_DNA.end());
            
            random_shuffle(all_DNA.begin(), all_DNA.end());
            
            return vector<int>(all_DNA.begin(), all_DNA.begin() + N_APS);
            
        } 
		else 
		{
            int i_ = rand() % cross_last_K;
            return pop[i_];
        }
    }

    vector<int> mutate(vector<int> child) 
	{
        for (int point = 0; point < DNA_size; ++point) 
		{
            if ((double)rand() / RAND_MAX < mutate_rate) 
			{
                vector<int> main_list = candidate;
                
                for (auto& c : child) 
				{
                    main_list.erase(remove(main_list.begin(), main_list.end(), c), main_list.end());
                }
                
                int swap_point = main_list[rand() % main_list.size()];
                
                child[point] = swap_point;
            }
        }
        
        return child;
    }

    void evolve(const vector<double>& fitness) 
	{
        vector<int> idx(fitness.size());
        
        iota(idx.begin(), idx.end(), 0);
        
        sort(idx.begin(), idx.end(), [&](int i1, int i2) { return fitness[i1] < fitness[i2]; });
        
        vector<int> least_idx(idx.end() - cross_last_K, idx.end());
        
        vector<int> most_idx(idx.begin(), idx.begin() + cross_last_K);
        
        vector<vector<int>> pop_copy = pop;
        
        for (auto& parent : least_idx) 
		{
            vector<int> child = crossover(pop[parent], pop_copy);
            
            child = mutate(child);
            
            pop[parent] = child;
        }
    }

private:
	
    int DNA_size;
    double cross_rate;
    double mutate_rate;
    int pop_size;
    
    vector<int> candidate;
    vector<vector<int>> pop;
    vector<vector<double>> RSSI_MAP;
};

class TSP 
{
public:
    TSP(int n_) 
	{
        RSSI_MAP = normalize(load_RSSI());
        // reshape RSSI_MAP to M_Candidate x 46 x 98
        // ...
        namedWindow("TSP", WINDOW_NORMAL);
    }

    void plotting(const vector<int>& best_idx, double fitness) 
	{
        Mat output = mean(RSSI_MAP, best_idx);
        
        Mat img;
        
        normalize(output, img, 0, 255, NORM_MINMAX);
        
        applyColorMap(img, img, COLORMAP_JET);
        
        putText(img, "Total distance=" + to_string(fitness), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        
        imshow("TSP", img);
        
        waitKey(1);
    }

private:
    vector<vector<vector<double>>> RSSI_MAP;
};


int main() 
{
    GA ga(N_APS, CROSS_RATE, MUTATE_RATE, POP_SIZE);
    TSP env(N_APS);

    auto start_time = chrono::high_resolution_clock::now();

    for (int generation = 0; generation < N_GENERATIONS; ++generation) {
        auto DNA = ga.translateDNA(ga.pop);
        auto fitness = ga.get_fitness(DNA);
        ga.evolve(fitness);
        auto best_idx = min_element(fitness.begin(), fitness.end()) - fitness.begin();
        cout << "Gen: " << generation << " | best fit: " << fitness[best_idx] << " | worst fit: " << *max_element(fitness.begin(), fitness.end()) << " | avg fit: " << accumulate(fitness.begin(), fitness.end(), 0.0) / fitness.size() << endl;
        auto [fitness_, r_mean, r_variance, P_eig_values, entropy, P] = fitness_f(ga.RSSI_MAP[ga.pop[best_idx]]);
        cout << "Configuration: " << ga.pop[best_idx] << " mean: " << r_mean << " variance: " << r_variance << " P_eig_values: " << P_eig_values << " entropy: " << entropy << " P: " << P << endl;
        env.plotting(ga.pop[best_idx], fitness[best_idx]);
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();

    cout << "elapsed time: " << elapsed_time << "s" << endl;

    return 0;
}
