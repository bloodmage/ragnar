#pragma once

#include <string>
#include "StorageInterfaceBase.h"

namespace libtorrent
{
    struct add_torrent_params;
}

namespace Ragnar
{
    ref class TorrentInfo;
	ref class SHA1Hash;

    using namespace System;
    using namespace System::Collections::Generic;

    public ref class AddTorrentParams
    {
    private:
        libtorrent::add_torrent_params* _params;
        cli::array<byte>^ _resumeData;
        TorrentInfo^ _info;
        IList<String^>^ _trackers;
        IList<String^>^ _urlSeeds;
		Unsafe::StorageFuncParam^ storage;
		SHA1Hash^ infohash;

    internal:
        libtorrent::add_torrent_params& get_ptr() { return *_params; }
        AddTorrentParams(const libtorrent::add_torrent_params &params);

    public:
        AddTorrentParams();
        ~AddTorrentParams();

        property IList<String^>^ Trackers { IList<String^>^ get(); }

        property IList<String^>^ UrlSeeds { IList<String^>^ get(); }

        property String^ Name
        {
            String^ get();
            void set(String^ value);
        }

        property String^ SavePath
        {
            void set(String^ value);
            String^ get();
        }

        property String^ Url
        {
            void set(String^ value);
            String^ get();
        }

        property TorrentInfo^ TorrentInfo
        {
            void set(Ragnar::TorrentInfo^ value);
            Ragnar::TorrentInfo^ get();
        }
		
		property SHA1Hash^ InfoHash { SHA1Hash^ get(); void set(SHA1Hash^ value); }

        property cli::array<byte>^ ResumeData
        {
            cli::array<byte>^ get();
            void set(cli::array<byte>^ value);
        }

		property Unsafe::StorageFuncParam^ Storage
		{
			Unsafe::StorageFuncParam^ get();
			void set(Unsafe::StorageFuncParam^ value);
		}
		
        property int MaxUploads { int get(); void set(int value); }

        property int MaxConnections { int get(); void set(int value); }

        property int UploadLimit { int get(); void set(int value); }

        property int DownloadLimit { int get(); void set(int value); }

        property bool SeedMode 
        {
            void set(bool value);
            bool get();
        }

		static AddTorrentParams^ FromMagnetUri(System::String^ uri);
		static AddTorrentParams^ FromTorrentInfo(Ragnar::TorrentInfo^ info);
		static AddTorrentParams^ FromInfoHash(SHA1Hash^ hash);
	};
}
