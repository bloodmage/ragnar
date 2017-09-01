#include "stdafx.h"
#include "FileEntry.h"
#include "TorrentInfo.h"
#include "Utils.h"
#include "FileStorage.h"
#include "SHA1Hash.h"
#include "Entry.h"
#include <libtorrent\torrent_info.hpp>

namespace Ragnar
{
	TorrentInfo::TorrentInfo(boost::shared_ptr<libtorrent::torrent_info> info)
	{
		this->_ptr = new boost::shared_ptr<libtorrent::torrent_info>(info);
		this->_info = this->_ptr->get();
	}

	TorrentInfo::TorrentInfo(boost::shared_ptr<const libtorrent::torrent_info> info)
	{
		this->_ptr = new boost::shared_ptr<libtorrent::torrent_info>(*reinterpret_cast<boost::shared_ptr<libtorrent::torrent_info>*>(&info));
		this->_info = this->_ptr->get();
	}

	TorrentInfo::TorrentInfo(System::String^ fileName)
    {
		this->_ptr = new boost::shared_ptr<libtorrent::torrent_info>(new libtorrent::torrent_info(Utils::GetStdStringFromManagedString(fileName)));
		this->_info = this->_ptr->get();
	}

    TorrentInfo::TorrentInfo(cli::array<byte>^ buffer)
    {
        pin_ptr<unsigned char> ptr = &buffer[0];
        const char *pbegin = (const char*)(const unsigned char*)ptr;

		this->_ptr = new boost::shared_ptr<libtorrent::torrent_info>(new libtorrent::torrent_info(pbegin, buffer->Length));
		this->_info = this->_ptr->get();
	}

	TorrentInfo::TorrentInfo(Unsafe::Entry^ entry)
	{
		this->_ptr = new boost::shared_ptr<libtorrent::torrent_info>(new libtorrent::torrent_info(*entry->entry));
		this->_info = this->_ptr->get();
	}
    TorrentInfo::~TorrentInfo()
    {
		if (this->_ptr != NULL)
		{
			delete this->_ptr;
			this->_ptr = NULL;
		}
    }

	FileStorage^ TorrentInfo::Files(){ return gcnew FileStorage(this->_info->files()); }
	FileStorage^ TorrentInfo::OrigFiles(){ return gcnew FileStorage(this->_info->orig_files()); }

	void TorrentInfo::RenameFile(int fileIndex, System::String^ fileName)
    {
        this->_info->rename_file(fileIndex, Utils::GetStdStringFromManagedString(fileName));
    }

    void TorrentInfo::AddTracker(System::String^ url)
    {
        this->AddTracker(url, 0);
    }

    void TorrentInfo::AddTracker(System::String^ url, int tier)
    {
        this->_info->add_tracker(Utils::GetStdStringFromManagedString(url), tier);
    }

    int TorrentInfo::NumPieces::get()
    {
        return this->_info->num_pieces();
    }

    long long TorrentInfo::TotalSize::get()
    {
        return this->_info->total_size();
    }

    int TorrentInfo::PieceLength::get()
    {
        return this->_info->piece_length();
    }

    SHA1Hash^ TorrentInfo::InfoHash::get()
    {
        return gcnew SHA1Hash(this->_info->info_hash());
    }

    int TorrentInfo::NumFiles::get()
    {
        return this->_info->num_files();
    }

    FileEntry^ TorrentInfo::FileAt(int index)
    {
        return gcnew FileEntry(this->_info->file_at(index));
    }

    System::String^ TorrentInfo::SslCert::get()
    {
		return Utils::GetManagedStringFromStandardString(this->_info->ssl_cert());
    }

    bool TorrentInfo::IsValid::get()
    {
        return this->_info->is_valid();
    }

    bool TorrentInfo::Private::get()
    {
        return this->_info->priv();
    }

    int TorrentInfo::PieceSize(int index)
    {
        return this->_info->piece_size(index);
    }

    System::Nullable<DateTime> TorrentInfo::CreationDate::get()
    {
        auto date = this->_info->creation_date();

        if (!date)
        {
            return Nullable<DateTime>();
        }

        return Utils::GetDateTimeFromTimeT(date.get());
    }

    System::String^ TorrentInfo::Name::get()
    {
		return Utils::GetManagedStringFromStandardString(this->_info->name());
    }

    System::String^ TorrentInfo::Comment::get()
    {
		return Utils::GetManagedStringFromStandardString(this->_info->comment());
    }

    System::String^ TorrentInfo::Creator::get()
    {
		return Utils::GetManagedStringFromStandardString(this->_info->creator());
    }

    int TorrentInfo::MetadataSize::get()
    {
        return this->_info->metadata_size();
    }

    bool TorrentInfo::IsMerkleTorrent::get()
    {
        return this->_info->is_merkle_torrent();
    }
}
