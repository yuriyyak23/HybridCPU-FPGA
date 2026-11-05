#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <limits>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	class AcceleratorShapeCapability final : public std::enable_shared_from_this<AcceleratorShapeCapability>
	{
	private:
		const std::wstring ShapeKind;
		const unsigned long long MinElements;
		const unsigned long long MaxElements;
		const unsigned char MinRank;
		const unsigned char MaxRank;

	public:
		AcceleratorShapeCapability(const std::wstring &shapeKind, unsigned long long minElements, unsigned long long maxElements, unsigned char minRank = 0, unsigned char maxRank = std::numeric_limits<unsigned char>::max());

		std::wstring getShapeKind() const;

		unsigned long long getMinElements() const;

		unsigned long long getMaxElements() const;

		unsigned char getMinRank() const;

		unsigned char getMaxRank() const;

		bool SupportsShape(const std::wstring &shapeKind, unsigned long long elementCount, unsigned char rank = 0);
	};

}
