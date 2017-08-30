#pragma once
#include "FileStorage.h"
#include "Utils.h"
#include "Entry.h"

#include <libtorrent/create_torrent.hpp>

namespace Ragnar
{

	public ref class CreateTorrent
	{
	internal:
		libtorrent::create_torrent * ptr;
		FileStorage^ storage;
	public:
		CreateTorrent(FileStorage^ storage) { this->storage = storage; ptr = new libtorrent::create_torrent(*storage->stor); }
		~CreateTorrent() { if (ptr != NULL) { free(ptr); ptr = NULL; storage->~FileStorage(); } }
		!CreateTorrent() { this->~CreateTorrent(); }
		void AddTracker(System::String^ tracker, int tier) { this->ptr->add_tracker(Utils::GetStdStringFromManagedString(tracker), tier); }
		void AddTracker(System::String^ tracker) { this->ptr->add_tracker(Utils::GetStdStringFromManagedString(tracker), 0); }
		void SetCreator(System::String^ creator) {
			if (creator == nullptr) this->ptr->set_creator(NULL);
			else {
				auto o = Utils::GetStdStringFromManagedString(creator);
				this->ptr->set_creator(o.c_str());
			}
		}
		void SetComment(System::String^ comment) {
			if (comment == nullptr) this->ptr->set_comment(NULL);
			else {
				auto o = Utils::GetStdStringFromManagedString(comment);
				this->ptr->set_comment(o.c_str());
			}
		}
		void SetPieceHashes(System::String^ s) {
			auto hg = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(s);
			System::Runtime::InteropServices::Marshal::FreeHGlobal(hg);
			std::wstring p((wchar_t*)hg.ToPointer());
			libtorrent::set_piece_hashes(*ptr, p);
		}

		Unsafe::Entry^ Generate() {
			Unsafe::Entry^ ent = Unsafe::Entry::Wrap(ptr->generate());
			return ent;
		}
	};
}