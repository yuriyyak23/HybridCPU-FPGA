#include <stdexcept>
#include <cassert>
enum class SlotClass { AluClass, MatrixTileStreamClass };
enum class MatrixTileRuntimeResourceClass { None, MatrixTileMemory, MatrixTileCompute };
class MatrixTileResourceContour { public: static SlotClass ResolveSlotClass(MatrixTileRuntimeResourceClass); };
SlotClass MatrixTileResourceContour::ResolveSlotClass(MatrixTileRuntimeResourceClass resourceClass)
	{
        switch (resourceClass)
        {
        case MatrixTileRuntimeResourceClass::MatrixTileMemory:
            return SlotClass::MatrixTileStreamClass;
        case MatrixTileRuntimeResourceClass::MatrixTileCompute:
            return SlotClass::AluClass;
        default:
            throw std::invalid_argument("A MatrixTile runtime resource class is required.");
        }
    
}
int main() {
 assert(MatrixTileResourceContour::ResolveSlotClass(MatrixTileRuntimeResourceClass::MatrixTileMemory)==SlotClass::MatrixTileStreamClass);
 assert(MatrixTileResourceContour::ResolveSlotClass(MatrixTileRuntimeResourceClass::MatrixTileCompute)==SlotClass::AluClass);
 try { MatrixTileResourceContour::ResolveSlotClass(MatrixTileRuntimeResourceClass::None); return 1; } catch (const std::invalid_argument&) {}
}