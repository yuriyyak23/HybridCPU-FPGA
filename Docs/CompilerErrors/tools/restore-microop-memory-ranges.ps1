$ErrorActionPreference = 'Stop'
$path = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$text = [IO.File]::ReadAllText($path)
$broken = 'std::shared_ptr<virtual> const std::vector < (unsigned long long Address, unsigned long long Length);'
$replacement = @"
struct MemoryRange
        {
            unsigned long long Address;
            unsigned long long Length;
        };
    private:
        std::vector<MemoryRange> ReadMemoryRanges{};
        std::vector<MemoryRange> WriteMemoryRanges{};
    public:
        virtual const std::vector<MemoryRange>& getReadMemoryRanges() const { return ReadMemoryRanges; }
        virtual const std::vector<MemoryRange>& getWriteMemoryRanges() const { return WriteMemoryRanges; }
    protected:
        virtual void setReadMemoryRanges(const std::vector<MemoryRange>& value) { ReadMemoryRanges = value; }
        virtual void setWriteMemoryRanges(const std::vector<MemoryRange>& value) { WriteMemoryRanges = value; }
"@
if (-not $text.Contains($broken)) { throw 'Expected truncated declaration not found; no changes made.' }
$text = $text.Replace($broken, $replacement)
[IO.File]::WriteAllText($path, $text, [Text.UTF8Encoding]::new($false))
