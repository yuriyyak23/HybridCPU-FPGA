#pragma once

#include "InstructionClass.h"
#include "../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "SerializationClass.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Arch
{
	/// <summary>
	/// Maps <see cref="Processor.CPU_Core.InstructionsEnum"/> opcodes to their canonical ISA v4
	/// <see cref="InstructionClass"/> and <see cref="SerializationClass"/> values.
	/// <para>
	/// This is the single authoritative source for opcode-to-class mapping in the
	/// ISA v4 pipeline. All decoders, IR builders, and MicroOp constructors must use
	/// this classifier rather than maintaining independent mapping tables.
	/// </para>
	/// </summary>
	class InstructionClassifier final
	{
		/// <summary>
		/// Returns the canonical <see cref="InstructionClass"/> for the given opcode.
		/// </summary>
	public:
		static InstructionClass GetClass(std::shared_ptr<Processor::CPU_Core::IsaOpcode> opcode);

		static InstructionClass GetClass(unsigned short opcode);

		static InstructionClass GetClass(Processor::CPU_Core::InstructionsEnum opcode);

		/// <summary>
		/// Returns the canonical <see cref="SerializationClass"/> for the given opcode.
		/// </summary>
		static SerializationClass GetSerializationClass(std::shared_ptr<Processor::CPU_Core::IsaOpcode> opcode);

		static SerializationClass GetSerializationClass(unsigned short opcode);

		static SerializationClass GetSerializationClass(Processor::CPU_Core::InstructionsEnum opcode);

		/// <summary>
		/// Returns both the <see cref="InstructionClass"/> and <see cref="SerializationClass"/>
		/// for the given opcode in a single call.
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public static (InstructionClass Class, SerializationClass Serialization) Classify(Processor.CPU_Core.IsaOpcode opcode)
//		{
//			return (GetClass(opcode), GetSerializationClass(opcode));
//		}

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public static (InstructionClass Class, SerializationClass Serialization) Classify(ushort opcode)
//		{
//			return (GetClass(opcode), GetSerializationClass(opcode));
//		}

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public static (InstructionClass Class, SerializationClass Serialization) Classify(Processor.CPU_Core.InstructionsEnum opcode)
//		{
//			return (GetClass(opcode), GetSerializationClass(opcode));
//		}
	};
}
