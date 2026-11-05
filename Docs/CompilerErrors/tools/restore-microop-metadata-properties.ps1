$ErrorActionPreference = 'Stop'
$path = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$text = [IO.File]::ReadAllText($path)
$marker = 'virtual void setWriteMemoryRanges(const std::vector<MemoryRange>& value) { WriteMemoryRanges = value; }'
if (-not $text.Contains($marker) -or $text.Contains('int getOwnerThreadId() const { return OwnerThreadId; }')) { throw 'Unexpected header state; no changes made.' }
$addition = @"

    public:
        int getOwnerThreadId() const { return OwnerThreadId; }
        void setOwnerThreadId(int value) { OwnerThreadId = value; }
        int getOwnerContextId() const { return OwnerContextId; }
        void setOwnerContextId(int value) { OwnerContextId = value; }
        int getVirtualThreadId() const { return VirtualThreadId; }
        void setVirtualThreadId(int value) { VirtualThreadId = value; }
        virtual bool getHasSideEffects() const { return HasSideEffects; }
    protected:
        virtual void setHasSideEffects(bool value) { HasSideEffects = value; }
    public:
        int getQueueIndex() const { return QueueIndex; }
        void setQueueIndex(int value) { QueueIndex = value; }
        virtual MicroOpClass getClass() const { return Class; }
    protected:
        virtual void setClass(MicroOpClass value) { Class = value; }
    public:
        SlotPlacementMetadata getPlacement() const { return Placement; }
        void setPlacement(SlotPlacementMetadata value) { Placement = value; }
        unsigned long long getResourceToken() const { return ResourceToken; }
        void setResourceToken(unsigned long long value) { ResourceToken = value; }
        bool getScoreboardPending() const { return ScoreboardPending; }
        void setScoreboardPending(bool value) { ScoreboardPending = value; }
        bool isSpeculative() const { return IsSpeculative; }
        void setSpeculative(bool value) { IsSpeculative = value; }
        bool getFaulted() const { return Faulted; }
        void setFaulted(bool value) { Faulted = value; }

        void ApplyCanonicalDecodeProjection(Arch::InstructionClass instructionClass,
            Arch::SerializationClass serializationClass, SlotPlacementMetadata placement,
            bool isMemoryOp, bool isControlFlow, bool writesRegister,
            const std::vector<int>& readRegisters, const std::vector<int>& writeRegisters)
        {
            InstructionClass = instructionClass;
            SerializationClass = serializationClass;
            Placement = placement;
            IsMemoryOp = isMemoryOp;
            IsControlFlow = isControlFlow;
            WritesRegister = writesRegister;
            ReadRegisters = readRegisters;
            WriteRegisters = writeRegisters;
            _hasAdmissionMetadata = false;
        }
    protected:
        void SetPlacement(SlotClass requiredSlotClass, SlotPinningKind pinningKind,
            unsigned char pinnedLaneId = 0)
        {
            Placement.RequiredSlotClass = requiredSlotClass;
            Placement.PinningKind = pinningKind;
            Placement.PinnedLaneId = pinningKind == SlotPinningKind::HardPinned ? pinnedLaneId : 0;
        }
        void SetClassFlexiblePlacement(SlotClass requiredSlotClass)
        {
            SetPlacement(requiredSlotClass, SlotPinningKind::ClassFlexible);
        }
        void SetHardPinnedPlacement(SlotClass requiredSlotClass, unsigned char pinnedLaneId)
        {
            SetPlacement(requiredSlotClass, SlotPinningKind::HardPinned, pinnedLaneId);
        }
"@
$text = $text.Replace($marker, $marker + $addition)
[IO.File]::WriteAllText($path, $text, [Text.UTF8Encoding]::new($false))
