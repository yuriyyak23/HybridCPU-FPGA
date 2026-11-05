struct Processor { struct CPU_Core { enum class CorePowerState : int { C0_Active=0,C1_Halt=1,C2_StopClock=2,C3_Sleep=3,C6_DeepPowerDown=6,ErrorState=-1 }; }; }; class PowerControlCsr { public: static Processor::CPU_Core::CorePowerState DecodePowerState(unsigned long long); };
Processor::CPU_Core::CorePowerState PowerControlCsr::DecodePowerState(unsigned long long rawValue)
	{
        using State = Processor::CPU_Core::CorePowerState;
        switch (rawValue) {
            case static_cast<unsigned long long>(State::C0_Active): return State::C0_Active;
            case static_cast<unsigned long long>(State::C1_Halt): return State::C1_Halt;
            case static_cast<unsigned long long>(State::C2_StopClock): return State::C2_StopClock;
            case static_cast<unsigned long long>(State::C3_Sleep): return State::C3_Sleep;
            case static_cast<unsigned long long>(State::C6_DeepPowerDown): return State::C6_DeepPowerDown;
            default: return State::ErrorState;
        }
    }
int main(){using S=Processor::CPU_Core::CorePowerState; for(unsigned long long i=0;i<10;i++){ S x=PowerControlCsr::DecodePowerState(i); if(i==0 || i==1 || i==2 || i==3 || i==6){if(static_cast<unsigned long long>(x)!=i)return 1;} else if(x!=S::ErrorState)return 2;} if(PowerControlCsr::DecodePowerState(0x100000000ULL)!=S::ErrorState)return 3; return 0; }