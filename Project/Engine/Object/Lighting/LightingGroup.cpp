#include "LightingGroup.h"

LightingGroup* LightingGroup::instance_ = nullptr;

LightingGroup *LightingGroup::GetInstance()
{
	if(instance_ == nullptr){
		instance_ = new LightingGroup();
	}

	return instance_;
}
