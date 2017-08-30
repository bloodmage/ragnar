#pragma once
#include "libtorrent/storage_defs.hpp"
#include "libtorrent/error_code.hpp"
#include "ErrorCodeEnum.h"

using namespace System::Collections::Generic;
namespace Ragnar
{
	ref class FileStorage;
	namespace Unsafe
	{
		///<summary>
		///error on file operation
		///</summary>
		public enum class file_operation_t : int {
			none,
			stat,
			mkdir,
			open,
			rename,
			remove,
			copy,
			read,
			write,
			fallocate,
			alloc_cache_piece,
			partfile_move,
			partfile_read,
			partfile_write,
			check_resume,
			hard_link
		};

		struct _storageInterface;

		public ref class StorageError
		{
		private:
			libtorrent::storage_error* ec;
		public:
			StorageError(libtorrent::storage_error& ec){ this->ec = &ec; }

			property boost::int32_t File{boost::int32_t get(){ return this->ec->file; } void set(boost::int32_t value){ this->ec->file = value; }}
			property file_operation_t Operation{file_operation_t get(){ return file_operation_t((int)ec->operation); } void set(file_operation_t value){ ec->operation = (int)value; }}
			void MakeErrorCode(ErrorCodeEnum code){ this->ec->ec = libtorrent::errors::make_error_code((libtorrent::errors::error_code_enum)(int)code); }
			property int ErrorCode{int get(){ return this->ec->ec.value(); }}
		};

		public ref class StorageInterfaceBase abstract
		{
		public:
			StorageInterfaceBase();

		internal:
			_storageInterface*plugin;
			bool installed = false;
			~StorageInterfaceBase();
			!StorageInterfaceBase();
			void Unload();

			FileStorage^ filestorage;
		public:
			virtual bool Initialize(StorageError^ ret) = 0;
			//readv, writev
			virtual bool HasAnyFile(StorageError^ ret) = 0;
			virtual void SetFilePriority(const void* prio, StorageError^ ret) = 0;
			virtual int Read(char* buf, int slot, int offset, int size) = 0;
			virtual int Write(const char* buf, int slot, int offset, int size) = 0;
			virtual int MoveStorage(System::String^ savepath, int flags, StorageError^ ret) = 0;
			virtual bool VerifyResumeData(const void*rd, void*err, StorageError^ ret) = 0;
			virtual void WriteResumeData(void*rd, StorageError^ ret) = 0;
			virtual void ReleaseFiles(StorageError^ ret) = 0;
			virtual void RenameFile(int index, System::String^ new_filename, StorageError^ ret) = 0;
			virtual void DeleteFiles(int options, StorageError^ ret) = 0;
			virtual void FinalizeFile(int index, StorageError^ ret) = 0;
			virtual bool Tick(StorageError^ ret) { return false; }
			virtual void OnDestroy() { }

			property FileStorage^ Storage{ FileStorage^ get(); }
		};

		delegate libtorrent::storage_interface* storage_constructor_delegate(libtorrent::storage_params const& params);

		///<summary>
		///fs: filestorage specified by torrent
		///mapped: mapped filestorage
		///path: target path
		///</summary>
		public delegate StorageInterfaceBase^ StorageConstructor(FileStorage^ fs, System::String^ path);

		public ref class StorageFuncParam sealed
		{
		internal:
			StorageFuncParam(StorageConstructor^ constructor);
			libtorrent::storage_constructor_type* storage_constructor_func;
		private:
			libtorrent::storage_interface* MethodCall(libtorrent::storage_params const& params);
			storage_constructor_delegate^ methodptr;
			StorageConstructor^ constructor;

		public:
			StorageConstructor^ GetDelegate() { return constructor; }
		};

		public ref class StorageConstructorCache abstract sealed
		{
		private:
			static Dictionary<StorageConstructor^, StorageFuncParam^>^ paramDict = gcnew Dictionary<StorageConstructor^, StorageFuncParam^>();

		public:
			static StorageFuncParam^ GetFuncParamForStorageConstructor(StorageConstructor^ constructor);
		};
	}
}