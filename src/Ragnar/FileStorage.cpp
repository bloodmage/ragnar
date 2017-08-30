#include "Stdafx.h"
#include "FileStorage.h"
#include "FileEntry.h"

#include "libtorrent\file_storage.hpp"
#include "libtorrent\peer_request.hpp"
#include "Utils.h"
namespace libtorrent
{
	typedef __int64 size_type;
}
namespace Ragnar
{
	FileStorage::FileStorage(libtorrent::file_storage const&store)
	{
		stor = new libtorrent::file_storage(store);
		entries = gcnew array<FileEntry^, 1>(store.num_files());
	}
	
	FileStorage::~FileStorage()
	{
		if (stor != NULL)
		{
			delete stor;
			stor = NULL;
			entries = nullptr;
		}
	}

	FileStorage::!FileStorage() { this->~FileStorage(); }

	int FileStorage::NumFiles::get() { return stor->num_files(); }
	int FileStorage::NumPieces::get() { return stor->num_pieces(); }
	System::Int64 FileStorage::TotalSize::get() { return (System::Int64)stor->total_size(); }
	System::String^ FileStorage::Name::get() { return Utils::GetManagedStringFromStandardString(stor->name()); }
	PeerRequest FileStorage::MapFile(int file, System::Int64 offset, int size)
	{
		auto request = stor->map_file(file, (libtorrent::size_type)offset, size);
		PeerRequest ret;
		ret.Length = request.length;
		ret.Piece = request.piece;
		ret.Start = request.start;
		return ret;
	}
	System::Collections::Generic::List<FileSlice>^ FileStorage::MapBlock(int piece, System::Int64 offset, int size)
	{
		auto ret = gcnew System::Collections::Generic::List<FileSlice>();
		auto request = stor->map_block(piece, (libtorrent::size_type)offset, size);

		for (int i = 0; i < request.size(); i++)
		{
			FileSlice val;
			val.FileIndex = request[i].file_index;
			val.Offset = request[i].offset;
			val.Size = request[i].size;
			ret->Add(val);
		}

		return ret;
	}

	FileEntry^ FileStorage::At(int index)
	{
		if (index < 0 || index >= NumFiles) throw gcnew InvalidOperationException("Index out of range");
		return gcnew FileEntry(stor->at(index));
	}

	int FileStorage::PieceSize(int index) {
		return stor->piece_size(index);
	}

	int FileStorage::PieceLength::get() {
		return stor->piece_length();
	}
}