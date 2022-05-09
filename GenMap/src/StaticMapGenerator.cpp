#include "interface_lib.h"
#include "MapLoader.h"
#include "DefaultMap.h"

API_EXPORT void InitMap(SInfo* info)
{
	info->SetName("MA2S Map generation");
	info->AddMember("Steven Paterour");
	info->AddMember("Alexis Vie");
	info->AddMember("Souleymane Ouattara");
	info->AddMember("Malo Maisonneuve");

}

API_EXPORT IMap* MapFactory()
{
	return(new MapLoader());
}

API_EXPORT void MapDelete(IMap* obj)
{
	delete(obj);
}
