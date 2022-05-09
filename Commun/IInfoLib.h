#pragma once
// v 1.0

#include <cstddef>

// Structure définissant l'identification de la librairie
struct SInfo
{
	virtual ~SInfo() {}
	virtual void SetName(const char* name) = 0;
	virtual const char* GetName() const = 0;
	virtual void AddMember(const char* name) = 0;
	virtual std::size_t GetNbMembers() const = 0;
	virtual const char* GetMember(std::size_t i) const = 0;
};

// Types prédéfinis pour simplifier la récupération des fonctions dans la librairie
using pInitLib = void (*)(SInfo* info);
