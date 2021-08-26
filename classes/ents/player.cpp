#include "player.hpp"
#include "../../features/features.hpp"

std::mutex m;

void Player::UpdateHeldItems()
{
	// read active item identifier
	int active_weapon_id = rust->mem->Read<int>(this->ent + offsets->clActiveItem);

	// read items array
	uint64_t items = rust->mem->ReadChain<uint64_t>(this->ent, { (uint64_t)offsets->playerInventory, (uint64_t)offsets->containerBelt, (uint64_t)offsets->beltContentsList, 0x10 });

	// iterate over this->ents belt
	for (int items_on_belt = 0; items_on_belt <= 5; items_on_belt++)
	{
		// read current item
		uint64_t item = rust->mem->Read<uint64_t>(items + 0x20 + (items_on_belt * 0x8));

		// read the this->ents active weapon
		int active_weapon = rust->mem->Read<uint32_t>(item + 0x28);

		m.lock();
		this->held_items[items_on_belt].first = GetItemName(item);
		this->held_items[items_on_belt].second = item;
		m.unlock();

		if (active_weapon == 0) { this->held_items[items_on_belt].first = ""; continue; }

		// insert local players weapons into the weapons map
		if (features->weapons.find(item) == features->weapons.end() && this->local_player) {
			features->weapons[item] = HeldItem(item);
		}

		// check if the active weapon matches this iterations item
		if (active_weapon_id == active_weapon)
		{
			// set the held item

			m.lock();
			this->helditem = items_on_belt;
			m.unlock();
		}
	}
}