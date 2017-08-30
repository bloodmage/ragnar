#include "Stdafx.h"
#include "StorageInterfaceBase.h"
#include "Utils.h"
#include "FileStorage.h"
#include "libtorrent/storage.hpp"
#include <vcclr.h>
#include <msclr\lock.h>
using namespace Ragnar;
namespace libtorrent
{
	typedef __int64 size_type;
}

namespace Ragnar
{
	namespace Unsafe
	{

		struct _storageInterface : libtorrent::storage_interface
		{
		public:
			_storageInterface(StorageInterfaceBase^ obj) : libtorrent::storage_interface(), handle(obj) { }
			~_storageInterface() { handle->Unload(); }
			gcroot<StorageInterfaceBase^> handle;

			virtual void initialize(libtorrent::storage_error& ec) { handle->Initialize(gcnew StorageError(ec)); }
			//TODO: readv & writev
			virtual int readv(libtorrent::file::iovec_t const* bufs, int num_bufs
				, int piece, int offset, int flags, libtorrent::storage_error& ec) {
				int ret = 0;
				for (libtorrent::file::iovec_t const* i = bufs, *end(bufs + num_bufs); i < end; ++i)
				{
					int r = read((char*)i->iov_base, piece, offset, i->iov_len);
					offset += i->iov_len;
					if (r == -1) return -1;
					ret += r;
				}
				return ret;
			}
			virtual int writev(libtorrent::file::iovec_t const* bufs, int num_bufs
				, int piece, int offset, int flags, libtorrent::storage_error& ec) {
				int ret = 0;
				for (libtorrent::file::iovec_t const* i = bufs, *end(bufs + num_bufs); i < end; ++i)
				{
					int r = write((char const*)i->iov_base, piece, offset, i->iov_len);
					offset += i->iov_len;
					if (r == -1) return -1;
					ret += r;
				}
				return ret;
			}
			virtual bool has_any_file(libtorrent::storage_error& ec) { return handle->HasAnyFile(gcnew StorageError(ec)); }
			virtual void set_file_priority(std::vector<boost::uint8_t> const& prio, libtorrent::storage_error& ec) { handle->SetFilePriority(&prio, gcnew StorageError(ec)); }

			virtual int read(char* buf, int slot, int offset, int size) {
				int ret = handle->Read(buf, slot, offset, size);
				return ret;
			}
			virtual int write(const char* buf, int slot, int offset, int size) { 
				int ret = handle->Write(buf, slot, offset, size); 
				return ret;
			}

			virtual int move_storage(std::string const& save_path, int flags, libtorrent::storage_error& ec) { return handle->MoveStorage(Utils::GetManagedStringFromStandardString(save_path), flags, gcnew StorageError(ec)); }
			virtual bool verify_resume_data(libtorrent::bdecode_node const& rd, std::vector<std::string> const* links, libtorrent::storage_error& error) { return handle->VerifyResumeData(&rd, &links, gcnew StorageError(error)); }
			virtual void write_resume_data(libtorrent::entry& rd, libtorrent::storage_error& error) const { handle->WriteResumeData(&rd, gcnew StorageError(error)); }
			virtual void release_files(libtorrent::storage_error& error) { handle->ReleaseFiles(gcnew StorageError(error)); }
			virtual void rename_file(int index, std::string const& new_filename, libtorrent::storage_error& ec) { handle->RenameFile(index, Utils::GetManagedStringFromStandardString(new_filename.c_str()), gcnew StorageError(ec)); }

			virtual void delete_files(int options, libtorrent::storage_error& ec) { handle->DeleteFiles(options, gcnew StorageError(ec)); }
			virtual void finalize_file(int index, libtorrent::storage_error& ec) { handle->FinalizeFile(index, gcnew StorageError(ec)); }
		};

		StorageInterfaceBase::StorageInterfaceBase()
		{
			plugin = new _storageInterface(this);
		}

		StorageInterfaceBase::~StorageInterfaceBase()
		{
			if (plugin != NULL && !installed)
			{
				delete plugin;
				plugin = NULL;
				filestorage = nullptr;
			}
		}
		StorageInterfaceBase::!StorageInterfaceBase()
		{
			this->~StorageInterfaceBase();
		}

		void StorageInterfaceBase::Unload()
		{
			plugin = NULL;
			OnDestroy();
			filestorage = nullptr;
		}


		FileStorage^ StorageInterfaceBase::Storage::get()
		{
			return filestorage;
		}

		StorageFuncParam^ StorageConstructorCache::GetFuncParamForStorageConstructor(StorageConstructor^ constructor)
		{
			Dictionary<StorageConstructor^, StorageFuncParam^>^ paramsd = StorageConstructorCache::paramDict;
			msclr::lock l(paramsd);
			if (!paramsd->ContainsKey(constructor))
				paramsd->Add(constructor, gcnew StorageFuncParam(constructor));
			return (*paramsd)[constructor];
		}

		StorageFuncParam::StorageFuncParam(StorageConstructor^ constructor)
		{
			this->constructor = constructor;
			methodptr = gcnew storage_constructor_delegate(this, &StorageFuncParam::MethodCall);
			storage_constructor_func = new libtorrent::storage_constructor_type((libtorrent::storage_interface*(*)(libtorrent::storage_params const&))
				System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(methodptr).ToPointer());
		}

		libtorrent::storage_interface* StorageFuncParam::MethodCall(libtorrent::storage_params const& params)
		{
			StorageInterfaceBase^ ret = nullptr;
			auto fs_ = gcnew FileStorage(*params.files);
			if (constructor != nullptr)
				ret = constructor(fs_, Utils::GetManagedStringFromStandardString(params.path));
			if (ret == nullptr)
				return libtorrent::default_storage_constructor(params);
			ret->installed = true;
			ret->plugin->handle = ret;
			ret->filestorage = fs_;
			return ret->plugin;
		}
	}
}