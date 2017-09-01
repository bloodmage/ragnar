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
	}
	
	FileStorage::FileStorage()
	{
		stor = new libtorrent::file_storage();
	}

	FileStorage::~FileStorage()
	{
		if (stor != NULL)
		{
			delete stor;
			stor = NULL;
		}
	}

	FileStorage::!FileStorage() { this->~FileStorage(); }

	int FileStorage::NumFiles::get() { return stor->num_files(); }
	int FileStorage::NumPieces::get() { return stor->num_pieces(); }
	void FileStorage::NumPieces::set(int value) { stor->set_num_pieces(value); }
	System::Int64 FileStorage::TotalSize::get() { return (System::Int64)stor->total_size(); }
	System::String^ FileStorage::Name::get() { return Utils::GetManagedStringFromStandardString(stor->name()); }
	void FileStorage::Name::set(System::String^ value) {
		auto name = Utils::GetStdStringFromManagedString(value);
		stor->set_name(name);
	}
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

	void FileStorage::Reserve(int num_files) { stor->reserve(num_files); }

	__int64 seconds(DateTime dt)
	{
		DateTime REFERENCE = System::DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind::Utc);
		return (__int64)dt.Subtract(REFERENCE).TotalSeconds;
	}

	void FileStorage::AddFile(System::String^ name, boost::int64_t file_size, flags_t file_flags, DateTime mtime, System::String^ symlink_path) {
		auto namestr = Utils::GetStdStringFromManagedString(name);
		auto time = seconds(mtime);
		std::string spath = "";
		if (symlink_path != nullptr && symlink_path->Length > 0)
			spath = Utils::GetStdStringFromManagedString(symlink_path);
		stor->add_file(namestr, file_size, (int)file_flags, time, spath);
	}

	void FileStorage::RenameFile(int idx, System::String^ NewFilename)
	{
		auto namestr = Utils::GetStdStringFromManagedString(NewFilename);
		stor->rename_file(idx, namestr);
	}

	void FileStorage::Optimize(int pad_file_limit, int alignment, bool tail_padding)
	{
		stor->optimize(pad_file_limit, alignment, tail_padding);
	}

}