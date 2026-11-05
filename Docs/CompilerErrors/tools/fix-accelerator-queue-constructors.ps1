$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Queues\AcceleratorCommandQueue.cpp'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('AcceleratorDevice::AcceleratorDevice(const std::wstring &deviceId, bool isAvailable)', 'AcceleratorDevice::AcceleratorDevice(const std::wstring &deviceId, bool isAvailable) : DeviceId(deviceId), IsAvailable(isAvailable)')
$s = $s.Replace("`t`tDeviceId = deviceId;`r`n`t`tsetAvailable(isAvailable);", '')
$s = $s.Replace('std::shared_ptr<AcceleratorTelemetry> telemetry) : _telemetry(telemetry)', 'std::shared_ptr<AcceleratorTelemetry> telemetry) : Capacity(capacity), Device(device), _telemetry(telemetry)')
$s = $s.Replace('throw ArgumentOutOfRangeException(L"capacity", L"L7-SDC accelerator command queue capacity must be positive.");', 'throw std::out_of_range("L7-SDC accelerator command queue capacity must be positive.");')
$old = @'
		Capacity = capacity;
	if (device != null)
	{
		Device = device;
	}
	else
	{
		throw std::invalid_argument("device");
	}
'@ -replace "`r?`n", "`r`n"
if (!$s.Contains($old)) { throw 'Queue constructor body not found' }
$s = $s.Replace($old, "`t`tif (!device) throw std::invalid_argument(`"device`");")
$s = $s.Replace('ArgumentNullException::ThrowIfNull(request);', 'if (!request) throw std::invalid_argument("request");')
[IO.File]::WriteAllText($p, $s)
