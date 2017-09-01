#include "stdafx.h"
#include "BitField.h"

#include <libtorrent\bitfield.hpp>

namespace Ragnar
{
	BitField::BitField(libtorrent::bitfield* field)
	{
		this->field = field;
	}

	bool BitField::AllTrue::get()
	{
		if (this->field->empty())
		{
			return false;
		}

		return this->field->all_set();
	}

	bool BitField::default::get(int i)
	{
		if (this->field->empty())
		{
			return false;
		}

		return this->field->get_bit(i);
	}
}