#include "stdafx.h"
#include "FileEntry.h"
#include "SHA1Hash.h"
#include "Utils.h"

#include <libtorrent\file_storage.hpp>

namespace Ragnar
{
    FileEntry::FileEntry(const libtorrent::file_entry &entry)
    {
        this->_entry = new libtorrent::file_entry(entry);
    }

    FileEntry::~FileEntry()
    {
        delete this->_entry;
    }

    System::String^ FileEntry::Path::get()
    {
        return Utils::GetManagedStringFromStandardString(this->_entry->path);
    }

    long long FileEntry::Offset::get()
    {
        return this->_entry->offset;
    }

    long long FileEntry::Size::get()
    {
        return this->_entry->size;
    }

    long long FileEntry::FileBase::get()
    {
        return this->_entry->file_base;
    }

	SHA1Hash^ FileEntry::FileHash::get()
	{
		return gcnew SHA1Hash(this->_entry->filehash);
	}

	bool FileEntry::PadFile::get()
	{
		return this->_entry->pad_file;
	}
}