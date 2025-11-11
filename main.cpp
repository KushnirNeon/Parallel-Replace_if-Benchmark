// g++ compiler

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <algorithm>
#include <execution>
#include <chrono>
#include <thread>

using namespace std;

ofstream outputFile("output.txt");

// replacement value for std::replace_if
int NEW_VALUE = -1;

// pred for std::replace_if
bool isOdd (int num)
{
    return ((num % 2) == 1);
}

template <class F>
void timeit (F && f, const string & policy)
{
    auto time1 = chrono::system_clock::now();
    f();
    auto time2 = chrono::system_clock::now();
    auto tdiff = chrono::duration_cast <chrono::milliseconds> (time2 - time1);
    outputFile << policy << ": " << tdiff.count() << "ms\n";
}

int main()
{
    // create vector of 10 mln random values
    const long long n = 10000000LL;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 9);
    vector <int> data;
    for (int i = 0; i < n; i++)
    {
        data.push_back(dis(gen));
    }

    // no policy
    vector <int> vector_no (data);
    timeit ([&vector_no]() {
        replace_if (vector_no.begin(), vector_no.end(), isOdd, NEW_VALUE);
    }, "No policy");


    // with different policies
    // sequential
    vector <int> vector_seq (data);
    timeit ([&vector_seq]() {
        replace_if (execution::seq,vector_seq.begin(), vector_seq.end(), isOdd, NEW_VALUE);
    }, "Sequential");

    // parallel
    vector <int> vector_par (data);
    timeit ([&vector_par]() {
        replace_if (execution::par,vector_par.begin(), vector_par.end(), isOdd, NEW_VALUE);
    }, "Parallel");

    // unsequential
    vector <int> vector_unseq (data);
    timeit ([&vector_unseq]() {
        replace_if (execution::unseq,vector_unseq.begin(), vector_unseq.end(), isOdd, NEW_VALUE);
    }, "Unsequential");

    // parallel unsequential
    vector <int> vector_par_unseq (data);
    timeit ([&vector_par_unseq]() {
        replace_if (execution::par_unseq,vector_par_unseq.begin(), vector_par_unseq.end(), isOdd, NEW_VALUE);
    }, "Parallel unsequential");


    // own algorithm
    int best_K = 1;
    auto best_time = chrono::milliseconds(100000);
    const int numThreads = 40;
    int chunkSize, start, end;
    outputFile << "Own parallel algorithm:\n";
    for (int K = 1; K <= numThreads; K++)
    {
        vector <int> vector_own (data);
        vector <thread> threads;
        chunkSize = n / K;
        auto time1 = chrono::system_clock::now();
        for (int i = 0; i < K; i++)
        {
            start = i * chunkSize;
            end = (i == K - 1) ? n : (i + 1) * chunkSize;
            threads.emplace_back ([&vector_own, start, end]() {
                for (int i = start; i < end; i++)
                {
                    if (isOdd(vector_own[i]))
                    {
                        vector_own[i] = NEW_VALUE;
                    }
                }
            });
        }

        for (auto & thread : threads)
        {
            thread.join();
        }

        auto time2 = chrono::system_clock::now();
        auto tdiff = chrono::duration_cast <chrono::milliseconds>(time2 - time1);
        outputFile << "K = " << K << ": " << tdiff.count() << "ms\n";
        if (tdiff < best_time)
        {
            best_K = K;
            best_time = tdiff;
        }
    }

    outputFile << "The best speed at K = " << best_K;
    outputFile << "\nThe number of processor threads: " << thread::hardware_concurrency();
    outputFile << "\nThese values are close";
    outputFile << "\nAs K increases, the time sometimes increases, then decreases. There is no clear trend";
    outputFile.close();

    return 0;
}
