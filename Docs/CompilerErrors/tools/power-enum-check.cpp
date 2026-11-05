enum class CorePowerState : unsigned int
			{
				C0_Active = 0, // Operating state, fully powered and executing
				C1_Halt = 1, // Halted, clock gated but powered, fast wake-up
				C2_StopClock = 2, // Clocks gated to more components, longer wake-up than C1
				C3_Sleep = 3, // Deeper sleep, caches may be flushed, longer wake-up
				C6_DeepPowerDown = 6, // Core voltage reduced or off, significant wake-up latency
				// P-states (Performance States) - can be combined or managed separately
				P0_MaxPerformance = 100, // Max frequency and voltage
				P1_HighPerformance = 101,
				Pn_MinPerformance = 115, // Min frequency and voltage for power saving
				ErrorState = 0xFFFFFFFF // Indicates an issue with power state transition
			};
static_assert(sizeof(CorePowerState)==4); static_assert(static_cast<unsigned int>(CorePowerState::ErrorState)==0xFFFFFFFFu); static_assert(static_cast<unsigned int>(CorePowerState::C6_DeepPowerDown)==6u); int main(){return 0;}