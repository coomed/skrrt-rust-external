#ifndef _ORE_HPP
#define _ORE_HPP

// includes
#include <cstdint>
#include "../../utils/memory/memory.hpp"
#include "../../utils/utils.hpp"
#include "../../skCrypt.h"
#include "player.hpp"

// ore class
class Ore
{
public:
	// default constructor
	Ore() {}

	// used to initialize an ore object
	Ore(uint64_t _ent)
	{
		// store constructor arguments
		this->ent = _ent;
		this->obj = rust->mem->ReadChain<uint64_t>(this->ent, { 0x10, 0x30 });

		// get the ore's position
		this->position = rust->mem->ReadChain<Vector3>(this->obj, { 0x30, 0x8, 0x38, 0x90 });

		// read the native name of the object
		name = rust->mem->ReadNative(this->obj + 0x60);

		// make the ore names clean
		if (name.find(std::string(skCrypt("sulfur-ore"))) != std::string::npos || name.find(std::string(skCrypt("ore_sulfur"))) != std::string::npos)
			name = std::string(skCrypt("Sulfur Ore"));

		else if (name.find(std::string(skCrypt("metal-ore"))) != std::string::npos || name.find(std::string(skCrypt("ore_metal"))) != std::string::npos)
			name = std::string(skCrypt("Metal Ore"));

		else if (name.find(std::string(skCrypt("stone-ore"))) != std::string::npos || name.find(std::string(skCrypt("ore_stone"))) != std::string::npos)
			name = std::string(skCrypt("Stone Ore"));

		else if (name.find(std::string(skCrypt("hemp"))) != std::string::npos)
			name = std::string(skCrypt("Hemp"));
		else if (name.find(std::string(skCrypt("stash"))) != std::string::npos)
			name = std::string(skCrypt("Stash"));
		else
			name = std::string(skCrypt("Invalid"));
	}

	// updates the modafucking ore
	bool Update()
	{
		return true;
	}

	// used to calculate the distance to another player
	int Distance(Player* player)
	{
		return this->position.Distance(player->position);
	}

public:
	uint64_t	ent;			// The BaseEntity address
	uint64_t	obj;			// The GameObject address
	Vector3		position;
	std::string name;
};

#endif // _ORE_HPP