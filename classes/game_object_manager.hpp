#ifndef _GOM_HPP
#define _GOM_HPP

// includes for this header
#include "../utils/memory/memory.hpp"

// class for interacting with the gameobjectmanager
class GameObjectManager
{
public:

	// constructor for this cssssslass
	GameObjectManager()
	{
		if (!rust->mem) return;

		this->gom_addr = rust->mem->Read<uint64_t>(rust->mem->up_addy + offsets->gameObjectManager);
	}

	// used to get the "camera" address
	uint64_t GetCamera()
	{
		// read into the first entry in the list
		if (this == nullptr) return 0;

		uint64_t address = rust->mem->Read<uint64_t>(this->gom_addr + 0x8);

		// loop until we hit tag 5, which is camera
		while (true)
		{
			// read into the GameObject
			uint64_t game_object = rust->mem->Read<uint64_t>(address + 0x10);

			// read this object's tag
			int16_t tag = rust->mem->Read<int16_t>(game_object + 0x54);

			if (tag == 5)
			{
				return rust->mem->ReadChain<uint64_t>(game_object, { 0x30, 0x18 });
			}

			// read into the next entry
			address = rust->mem->Read<uint64_t>(address + 0x8);
		}

		return address;
	}

	// used to get the "sky" address
	uint64_t GetSky()
	{
		if (this == nullptr) return 0;

		// read into the first entry in the list
		uint64_t address = rust->mem->Read<uint64_t>(this->gom_addr + 0x8);

		// loop until we hit tag 20011, which is sky dome
		while (true)
		{
			// read into the GameObject
			uint64_t game_object = rust->mem->Read<uint64_t>(address + 0x10);

			// read this object's tag
			int16_t tag = rust->mem->Read<int16_t>(game_object + 0x54);

			// check for sky dome tag
			if (tag == 20011)
			{
				// camera was found, return the address 
				return game_object;
			}

			// read into the next entry
			address = rust->mem->Read<uint64_t>(address + 0x8);
		}

		return address;
	}

public:

	// The GameObjectManager address
	uint64_t gom_addr = 0;
};

// gameobjectmanager extern
extern GameObjectManager* gom;

#endif // _GOM_HPP