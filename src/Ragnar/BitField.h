#pragma once

namespace libtorrent
{
	struct bitfield;
}

namespace Ragnar
{
	public ref class BitField
	{
	private:
		libtorrent::bitfield* field;
	internal:
		BitField(libtorrent::bitfield* field);
	public:
		property bool AllTrue { bool get();  }
		property bool default[int]
		{
			bool get(int);
		}
	};
}