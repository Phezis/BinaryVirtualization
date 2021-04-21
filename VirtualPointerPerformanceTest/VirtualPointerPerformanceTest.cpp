#include "VirtualPointer.h"
#include "Test.h"

#include <cstddef>
#include <iostream>
#include <chrono>
#include <memory>
#include <exception>
#include <fstream>
#include <algorithm>

namespace 
{
    constexpr char AVOID_OPTIMIZATION_FILE_NAME[] = "avoid_optimization.csv";
#if defined(_M_X64) || defined(__amd64__)
	constexpr char OUT_FILE_NAME[] = "results_x64.csv";
	constexpr char ARCHITECTURE[] = "x64";
#else
    constexpr char OUT_FILE_NAME[] = "results_x86.csv";
    constexpr char ARCHITECTURE[] = "x86";
#endif
}

using namespace std;
using namespace std::chrono;

int main()
{

    try
    {
        fstream resultsStream;
        fstream avoidOptimizationStream;
        try
        {
            resultsStream.open(OUT_FILE_NAME, ios_base::out | ios_base::binary);
            avoidOptimizationStream.open(AVOID_OPTIMIZATION_FILE_NAME, ios_base::out | ios_base::binary);

            using stream = std::pair<int, int>;
            constexpr auto repeatsInside = 1;
            constexpr auto repeatsOutside = 4;
            const auto depths = { 1, 2, 4 };
        	const auto streamConstitutes = {
            	stream{5120, 1000000},
                stream{51200, 100000},
                stream{512000, 10000},
                stream{5120000, 1000}
            };
            const auto packetSizes = {
                "5 кб",
                "50 кб",
                "500 кб",
                "5 мб"
            };
            resultsStream << ARCHITECTURE << std::endl;
            for (auto j = 0; j < repeatsOutside; ++j)
            {
                const auto resultsCount = depths.size() * streamConstitutes.size();
                unique_ptr<double[]> copyResults(new double[resultsCount]);
                unique_ptr<double[]> vptrResults(new double[resultsCount]);

                std::fill_n(copyResults.get(), resultsCount, std::numeric_limits<double>::max());
                std::fill_n(vptrResults.get(), resultsCount, std::numeric_limits<double>::max());

                Test test(avoidOptimizationStream, cout);
                test.setHeaderSize(4);
                test.setPayloadSize(184);

                auto resultId = 0;
                for (const auto& depth : depths)
                {
                    test.setDepth(depth);
                    for (const auto& streamConstitute : streamConstitutes)
                    {
                        test.setSummaryPacketsSize(streamConstitute.first);
                        test.setPacketsCount(streamConstitute.second);

                        for (auto i = 0; i < repeatsInside; ++i)
                        {
                            test.run();
                            copyResults[resultId] = std::min(copyResults[resultId], test.getCopyDuration().count());
                            vptrResults[resultId] = std::min(vptrResults[resultId], test.getVptrDuration().count());
                        }
                        std::cout << "Depth = " << depth << std::endl;
                        std::cout << "Packet bytesRemaining = " << streamConstitute.first << std::endl;
                        std::cout << "\tCopy time = " << copyResults[resultId] << std::endl;
                        std::cout << "\tVptr time = " << vptrResults[resultId] << std::endl;

                        ++resultId;
                    }
                }

                // print Depth-line
                resultsStream << ";";
                std::cout << ";";
                for (const auto& depth : depths)
                {
                    resultsStream << "Глубина " << depth;
                    std::cout << "Глубина " << depth;
                    for (size_t i = 0; i < streamConstitutes.size(); ++i)
                    {
                        resultsStream << ";";
                        std::cout << ";";
                    }
                }
                resultsStream << std::endl;
                std::cout << std::endl;
                // print packet bytesRemaining line
                resultsStream << ";";
                std::cout << ";";
                for (size_t i = 0; i < depths.size(); ++i)
                {
                    for (const auto& packetSize : packetSizes)
                    {
                        resultsStream << packetSize << ";";
                        std::cout << packetSize << ";";
                    }
                }
                resultsStream << std::endl;
                std::cout << std::endl;
                // print copy times
                resultsStream << "Копирование;";
                std::cout << "Копирование;";
                for (size_t i = 0; i < depths.size(); ++i)
                {
                    for (size_t j = 0; j < streamConstitutes.size(); ++j)
                    {
                        resultsStream << copyResults[i * depths.size() + j] << ";";
                        std::cout << copyResults[i * depths.size() + j] << ";";
                    }
                }
                resultsStream << std::endl;
                std::cout << std::endl;
                // print vptr times
                resultsStream << "Виртуализация;";
                std::cout << "Виртуализация;";
                for (size_t i = 0; i < depths.size(); ++i)
                {
                    for (size_t j = 0; j < streamConstitutes.size(); ++j)
                    {
                        resultsStream << vptrResults[i * depths.size() + j] << ";";
                        std::cout << vptrResults[i * depths.size() + j] << ";";
                    }
                }
                resultsStream << std::endl;
                std::cout << std::endl;
            }
        	
            resultsStream.close();
            avoidOptimizationStream.close();
        }
        catch (std::exception& e)
        {
            resultsStream.close();
            avoidOptimizationStream.close();
            std::cerr << "Unrecognized exception: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    catch (std::exception& e)
    {
        std::cerr << "Unrecognized exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}