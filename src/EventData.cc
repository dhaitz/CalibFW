#include "EventData.h"
#include "ZJetCuts.h"

using namespace CalibFW;

bool EventResult::IsValidEvent() const
{
		return (! IsCutInBitmask( JsonCut::CudId ) &&
				! IsCutInBitmask( HltCut::CudId));
}

unsigned long g_lTempNameAppend = 0;
