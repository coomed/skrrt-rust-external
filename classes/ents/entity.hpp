#ifndef _ENTITY_HPP
#define _ENTITY_HPP

// includes for this file
#include <cstdint>
#include <string>
#include <any>

// used to represent discovered entity
struct Entity
{
	std::uint64_t	   ent;
	std::string class_name;
};

#endif // _ENTITY_HPP