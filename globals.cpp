#include "utils/globals.hpp"

std::map<uint64_t, Player> Globals::get_players()
{
	this->players.mutex.lock();
	std::map<uint64_t, Player> copy = this->players.map;
	this->players.mutex.unlock();

	return copy;
}

void Globals::set_players(std::map<uint64_t, Player> map) {
	this->players.mutex.lock();
	this->players.map = map;
	this->players.mutex.unlock();
}

void Globals::clear_players() {
	this->ores.mutex.lock();
	this->ores.map.clear();
	this->ores.mutex.unlock();
}

std::map<uint64_t, Ore> Globals::get_ores()
{
	this->players.mutex.lock();
	std::map<uint64_t, Ore> copy = this->ores.map;
	this->players.mutex.unlock();

	return copy;
}

void Globals::set_ores(std::map<uint64_t, Ore> map) {
	this->ores.mutex.lock();
	this->ores.map = map;
	this->ores.mutex.unlock();
}

void Globals::clear_ores() {
	this->ores.mutex.lock();
	this->ores.map.clear();
	this->ores.mutex.unlock();
}