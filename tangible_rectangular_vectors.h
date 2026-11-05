#pragma once

// ----------------------------------------------------------------------------------------
// Copyright © 2004 - 2025 Tangible Software Solutions Inc.
// This class can be used by anyone provided that the copyright notice remains intact.
//
// This class includes methods to convert multidimensional arrays to C++ vectors.
// ----------------------------------------------------------------------------------------
#include <vector>
#include <memory>
namespace YAKSys_Hybrid_CPU::Execution { class NoC_XY_Router; }
namespace YAKSys_Hybrid_CPU::Core { enum class ScoreboardEntryType : unsigned char; }

class RectangularVectors
{
public:
    static std::vector<std::vector<bool>> rectangularBoolVector(int size1, int size2)
    {
        std::vector<std::vector<bool>> newVector(size1);
        for (int vector1 = 0; vector1 < size1; vector1++)
        {
            newVector[vector1] = std::vector<bool>(size2);
        }

        return newVector;
    }

    static std::vector<std::vector<int>> rectangularIntVector(int size1, int size2)
    {
        std::vector<std::vector<int>> newVector(size1);
        for (int vector1 = 0; vector1 < size1; vector1++)
        {
            newVector[vector1] = std::vector<int>(size2);
        }

        return newVector;
    }

    static std::vector<std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Execution::NoC_XY_Router>>> rectangularNoC_XY_RouterVector(int size1, int size2)
    {
        std::vector<std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Execution::NoC_XY_Router>>> newVector(size1);
        for (int vector1 = 0; vector1 < size1; vector1++)
        {
            newVector[vector1] = std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Execution::NoC_XY_Router>>(size2);
        }

        return newVector;
    }

    static std::vector<std::vector<YAKSys_Hybrid_CPU::Core::ScoreboardEntryType>> rectangularScoreboardEntryTypeVector(int size1, int size2)
    {
        std::vector<std::vector<YAKSys_Hybrid_CPU::Core::ScoreboardEntryType>> newVector(size1);
        for (int vector1 = 0; vector1 < size1; vector1++)
        {
            newVector[vector1] = std::vector<YAKSys_Hybrid_CPU::Core::ScoreboardEntryType>(size2);
        }

        return newVector;
    }
};
