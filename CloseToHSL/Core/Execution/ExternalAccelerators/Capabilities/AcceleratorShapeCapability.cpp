#include "AcceleratorShapeCapability.h"
#define NOMINMAX
#include <Windows.h>
#include <limits>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	AcceleratorShapeCapability::AcceleratorShapeCapability(const std::wstring &shapeKind, unsigned long long minElements, unsigned long long maxElements, unsigned char minRank, unsigned char maxRank) : ShapeKind(shapeKind), MinElements(minElements), MaxElements(maxElements), MinRank(minRank), MaxRank(maxRank)
	{
		if (StringHelper::isEmptyOrWhiteSpace(shapeKind))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Shape kind is required.", nameof(shapeKind));
			throw std::invalid_argument("Shape kind is required.");
		}

		if (minElements > maxElements)
		{
			throw std::out_of_range("Minimum elements must not exceed maximum elements.");
		}

		if (minRank > maxRank)
		{
			throw std::out_of_range("Minimum rank must not exceed maximum rank.");
		}

		
		
		
		
		
	}

	std::wstring AcceleratorShapeCapability::getShapeKind() const
	{
		return ShapeKind;
	}

	unsigned long long AcceleratorShapeCapability::getMinElements() const
	{
		return MinElements;
	}

	unsigned long long AcceleratorShapeCapability::getMaxElements() const
	{
		return MaxElements;
	}

	unsigned char AcceleratorShapeCapability::getMinRank() const
	{
		return MinRank;
	}

	unsigned char AcceleratorShapeCapability::getMaxRank() const
	{
		return MaxRank;
	}

	bool AcceleratorShapeCapability::SupportsShape(const std::wstring &shapeKind, unsigned long long elementCount, unsigned char rank)
	{
		if (StringHelper::isEmptyOrWhiteSpace(shapeKind))
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		return ShapeKind.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()) && shapeKind.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()) && CompareStringOrdinal(ShapeKind.data(), static_cast<int>(ShapeKind.size()), shapeKind.data(), static_cast<int>(shapeKind.size()), TRUE) == CSTR_EQUAL && elementCount >= MinElements && elementCount <= MaxElements && rank >= MinRank && rank <= MaxRank;
	}
}
