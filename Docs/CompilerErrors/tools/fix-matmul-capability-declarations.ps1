$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators'
$h = Join-Path $root 'Descriptors\MatMulDescriptor.h'
$s = [IO.File]::ReadAllText($h)
$s = $s.Replace('enum class MatMulLayoutFlags : int', 'enum class MatMulLayoutFlags : unsigned short')
$marker = "`tclass MatMulDescriptorValidator final"
$insert = @'
	class MatMulDescriptor;
	class MatMulFootprint;
	class MatMulDescriptorValidationResult;
	constexpr MatMulLayoutFlags operator|(MatMulLayoutFlags a, MatMulLayoutFlags b) noexcept {
		return static_cast<MatMulLayoutFlags>(static_cast<unsigned short>(a) | static_cast<unsigned short>(b));
	}
	constexpr MatMulLayoutFlags operator&(MatMulLayoutFlags a, MatMulLayoutFlags b) noexcept {
		return static_cast<MatMulLayoutFlags>(static_cast<unsigned short>(a) & static_cast<unsigned short>(b));
	}
	constexpr MatMulLayoutFlags operator~(MatMulLayoutFlags value) noexcept {
		return static_cast<MatMulLayoutFlags>(static_cast<unsigned short>(~static_cast<unsigned short>(value)));
	}
'@
if (!$s.Contains($marker)) { throw 'Validator boundary not found' }
$s = $s.Replace($marker, $insert + "`r`n" + $marker)
$s = $s.Replace('std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> &commandDescriptor', 'std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> commandDescriptor')
[IO.File]::WriteAllText($h, $s)
$cpp = Join-Path $root 'Descriptors\MatMulDescriptor.cpp'
$s = [IO.File]::ReadAllText($cpp)
$s = $s.Replace('std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> &commandDescriptor', 'std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> commandDescriptor')
[IO.File]::WriteAllText($cpp, $s)
$cpp = Join-Path $root 'Capabilities\MatMulCapabilityProvider.cpp'
$s = [IO.File]::ReadAllText($cpp)
$s = $s.Replace('std::shared_ptr<AcceleratorOperationCapability> operation = L"matmul", std::vector<std::wstring> {L"f32", L"f64", L"int32"}, std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities::AcceleratorShapeCapability>> {shape};', 'auto operation = std::make_shared<AcceleratorOperationCapability>(L"matmul", std::vector<std::wstring>{L"f32", L"f64", L"int32"}, std::vector<std::shared_ptr<AcceleratorShapeCapability>>{shape});')
$s = $s.Replace('CapabilityVersion, {operation}, MatMulResourceModel', 'CapabilityVersion, std::vector<std::shared_ptr<AcceleratorOperationCapability>>{operation}, MatMulResourceModel')
[IO.File]::WriteAllText($cpp, $s)
