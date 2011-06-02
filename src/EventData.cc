#include "EventData.h"
#include "ZJetCuts.h"

using namespace CalibFW;

bool EventResult::IsValidEvent()
{
		return (! IsCutInBitmask( JsonCut::CudId, this->m_cutBitmask ) &&
				! IsCutInBitmask( HltCut::CudId, this->m_cutBitmask ));
}

