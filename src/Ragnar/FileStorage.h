#pragma once

namespace libtorrent
{
	class file_storage;
}
namespace Ragnar
{
	ref class FileEntry;
	public value class PeerRequest
	{
	public:
		int Piece;
		int Start;
		int Length;
	};
	public value class FileSlice
	{
	public:
		int FileIndex;
		System::Int64 Offset;
		System::Int64 Size;
	};

	public ref class FileStorage
	{
	internal:
		libtorrent::file_storage* stor;
		array<FileEntry^, 1>^ entries;

		~FileStorage();
		!FileStorage();

	public:

		enum class flags_t
		{
			// the file is a pad file. It's required to contain zeros
			// at it will not be saved to disk. Its purpose is to make
			// the following file start on a piece boundary.
			pad_file = 1,

			// this file has the hidden attribute set. This is primarily
			// a windows attribute
			attribute_hidden = 2,

			// this file has the executable attribute set.
			attribute_executable = 4,

			// this file is a symbolic link. It should have a link
			// target string associated with it.
			attribute_symlink = 8
		};

		FileStorage(libtorrent::file_storage const&store);
		property int NumFiles { int get(); }
		property int NumPieces { int get(); void set(int n); }
		FileEntry^ At(int index);
		property System::Int64 TotalSize { System::Int64 get(); }
		property System::String^ Name { System::String^ get(); void set(System::String^ value); }
		int PieceSize(int index);
		property int PieceLength{int get(); }

		PeerRequest MapFile(int file, System::Int64 offset, int size);
		System::Collections::Generic::List<FileSlice>^ MapBlock(int piece, System::Int64 offset, int size);

		///<summary>
		///Preallocate buffer
		///</summary>
		void Reserve(int num_files);

		///<summary>
		///default: flags = 0, mtime = 0, symlink_path = null
		///</summary>
		void AddFile(System::String^ name, __int64 file_size, flags_t file_flags, System::DateTime mtime, System::String^ symlink_path);
		void RenameFile(int idx, System::String^ NewFilename);
		
		///<summary>
		///pad_file_limit: -1: nopad, x: file larger than x will be padded
		///alignment: where to pad, -1: piecesize, x: pad to x
		///tail_padding: false no pad to tail, true: pad tail (required for mutable torrents)
		///</summary>
		void Optimize(int pad_file_limit, int alignment, bool tail_padding);
	};
}