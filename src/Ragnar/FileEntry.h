#pragma once

namespace libtorrent
{
    struct file_entry;
}

namespace Ragnar
{
	ref class SHA1Hash;

    public ref class FileEntry
    {
    private:
        libtorrent::file_entry* _entry;

    internal:
        FileEntry(const libtorrent::file_entry &entry);

    public:
        ~FileEntry();

        property System::String^ Path { System::String^ get(); }

        // TODO: std::string symlink_path;

        property long long Offset { long long get(); }

        property long long Size { long long get(); }

        property long long FileBase { long long get(); }

        // TODO: std::time_t mtime;

        // sha1_hash filehash;
		property SHA1Hash^ FileHash{SHA1Hash^ get(); }

		property bool PadFile { bool get(); }


    };
}
