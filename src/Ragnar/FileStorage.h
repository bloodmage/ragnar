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
		FileStorage(libtorrent::file_storage const&store);
		property int NumFiles { int get(); }
		property int NumPieces { int get(); }
		FileEntry^ At(int index);
		property System::Int64 TotalSize { System::Int64 get(); }
		property System::String^ Name { System::String^ get(); }
		int PieceSize(int index);
		property int PieceLength{int get(); }

		PeerRequest MapFile(int file, System::Int64 offset, int size);
		System::Collections::Generic::List<FileSlice>^ MapBlock(int piece, System::Int64 offset, int size);

	};
}