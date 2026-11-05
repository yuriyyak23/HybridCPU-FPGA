$base = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2'
$h = Join-Path $base 'NonRTL\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.h'
$c = Join-Path $base 'NonRTL\Processor\Performance\YAKSys_Hybrid_CPU.PerformanceReport.cpp'
$s = [IO.File]::ReadAllText($h)
$s = $s.Replace('std::wstring MemoryCycleTelemetrySchemaVersion = std::wstring::Empty;', 'std::wstring MemoryCycleTelemetrySchemaVersion;')
$s = $s.Replace('std::shared_ptr<> &operator []();', 'const std::vector<double>& getPerThreadUtilization() const;`n`t`tvoid setPerThreadUtilization(const std::vector<double>& value);'.Replace('`n',"`r`n").Replace('`t',"`t"))
# The second invalid declaration belongs to a different property.
$needle='const std::vector<double>& getPerThreadUtilization() const;'+"`r`n`t`t"+'void setPerThreadUtilization(const std::vector<double>& value);'
$pos=$s.LastIndexOf($needle)
if ($pos -lt 0) { throw 'Property declaration not found' }
$s=$s.Remove($pos,$needle.Length).Insert($pos,('const std::vector<long long>& getPerVtInjections() const;'+"`r`n`t`t"+'void setPerVtInjections(const std::vector<long long>& value);'))
$s=$s.Replace('std::optional<ReplayEnvelopeReport> &envelopeReport = std::nullopt','const std::optional<ReplayEnvelopeReport> &envelopeReport = std::nullopt')
[IO.File]::WriteAllText($h,$s)
$s=[IO.File]::ReadAllText($c)
$s=$s.Replace('std::shared_ptr<> PerformanceReport::getPerThreadUtilization() const','const std::vector<double>& PerformanceReport::getPerThreadUtilization() const').Replace('setPerThreadUtilization(std::shared_ptr<> value)','setPerThreadUtilization(const std::vector<double>& value)').Replace('std::shared_ptr<> PerformanceReport::getPerVtInjections() const','const std::vector<long long>& PerformanceReport::getPerVtInjections() const').Replace('setPerVtInjections(std::shared_ptr<> value)','setPerVtInjections(const std::vector<long long>& value)').Replace('GeneratePhase4EvidenceSummary(std::optional<ReplayEnvelopeReport> &envelopeReport)','GeneratePhase4EvidenceSummary(const std::optional<ReplayEnvelopeReport> &envelopeReport)')
[IO.File]::WriteAllText($c,$s)