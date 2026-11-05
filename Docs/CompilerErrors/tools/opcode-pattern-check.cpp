enum class InstructionsEnum { MTILE_LOAD, MTILE_STORE, MTILE_MACC, MTRANSPOSE, OTHER };
bool memory(InstructionsEnum opcode) {		return (opcode == InstructionsEnum::MTILE_LOAD || opcode == InstructionsEnum::MTILE_STORE);}
bool compute(InstructionsEnum opcode) {		return (opcode == InstructionsEnum::MTILE_MACC || opcode == InstructionsEnum::MTRANSPOSE);}
int main() { using E=InstructionsEnum; return !(memory(E::MTILE_LOAD) && memory(E::MTILE_STORE) && !memory(E::MTILE_MACC) && compute(E::MTILE_MACC) && compute(E::MTRANSPOSE) && !compute(E::OTHER)); }