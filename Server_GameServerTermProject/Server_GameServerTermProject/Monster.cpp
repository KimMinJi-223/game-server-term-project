#include "Monster.h"

bool Monster::CASIsActive(bool expect, bool update)
{
	bool input = expect;
	return atomic_compare_exchange_strong(&_is_active, &input, update);
}
