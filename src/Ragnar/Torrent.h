#pragma once
#include "SHA1Hash.h"

#include "libtorrent\torrent.hpp"

namespace Ragnar
{
	namespace Unsafe
	{
		///<summary>
		///Thin wrapper around connection object, use with caution (incomplete)
		///</summary>
		public ref class Torrent
		{
		internal:
			libtorrent::torrent*t;
		public:
			Torrent(libtorrent::torrent*t) { this->t = t; }
			Ragnar::SHA1Hash^ InfoHash(){ return gcnew Ragnar::SHA1Hash(t->info_hash()); }
			property bool IpFilter{ bool get() { return this->t->apply_ip_filter(); } void set(bool value) { this->t->set_apply_ip_filter(value); } }
		};

	}
}