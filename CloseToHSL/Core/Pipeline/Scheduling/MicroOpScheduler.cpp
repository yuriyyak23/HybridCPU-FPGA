#include "MicroOpScheduler.h"

#include <codecvt>
#include <locale>

namespace YAKSys_Hybrid_CPU::Core
{

	PipelinePanicException::PipelinePanicException(const std::wstring &message) : std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message))
	{
	}
}


