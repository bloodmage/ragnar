#pragma once
#include <boost/shared_ptr.hpp>
namespace libtorrent
{
    class torrent_info;
}

namespace Ragnar
{
    ref class FileEntry;
	ref class FileStorage;
	ref class SHA1Hash;

    public ref class TorrentInfo
    {
    internal:
        boost::shared_ptr<libtorrent::torrent_info>* _ptr;
		libtorrent::torrent_info* _info;
		boost::shared_ptr<libtorrent::torrent_info> get_ptr() { return *this->_ptr; }
        //TorrentInfo(const libtorrent::torrent_info &info);

    public:
		TorrentInfo(boost::shared_ptr<libtorrent::torrent_info> info);
		///<summary>
		///Unsafe conversion
		///</summary>
		TorrentInfo(boost::shared_ptr<const libtorrent::torrent_info> info);
		TorrentInfo(System::String^ fileName);
        TorrentInfo(cli::array<byte>^ buffer);

        ~TorrentInfo();

        // TODO: file_storage const& files () const;
		FileStorage^ Files();
		// TODO: file_storage const& orig_files() const;
		FileStorage^ OrigFiles();

        void RenameFile(int fileIndex, System::String^ fileName);

        // TODO: void remap_files (file_storage const& f);
        // TODO: std::vector<announce_entry> const& trackers() const;

        void AddTracker(System::String^ url);
        void AddTracker(System::String^ url, int tier);

        // TODO: void add_url_seed (std::string const& url, std::string const& extern_auth = std::string(), web_seed_entry::headers_t const& extra_headers = web_seed_entry::headers_t());
        // TODO: std::vector<web_seed_entry> const& web_seeds() const;
        // TODO: void add_http_seed(std::string const& url, std::string const& extern_auth = std::string(), web_seed_entry::headers_t const& extra_headers = web_seed_entry::headers_t());
        
        property int NumPieces { int get(); }

        property long long TotalSize { long long get(); }

        property int PieceLength { int get(); }

		property SHA1Hash^ InfoHash { SHA1Hash^ get(); }

        property int NumFiles { int get(); }

        FileEntry^ FileAt(int index);

        // TODO: std::vector<file_slice> map_block(int piece, size_type offset, int size) const;
        // TODO: peer_request map_file(int file, size_type offset, int size) const;

        property System::String^ SslCert { System::String^ get(); }

        property bool IsValid { bool get(); }

        property bool Private { bool get(); }

        // TODO: bool is_i2p () const;
        // TODO: sha1_hash hash_for_piece(int index) const;
        // TODO: char const* hash_for_piece_ptr(int index) const;

        int PieceSize(int pieceIndex);

        // TODO: std::vector<sha1_hash> const& merkle_tree () const;
        // TODO: void set_merkle_tree(std::vector<sha1_hash>& h);

        property System::Nullable<System::DateTime> CreationDate { System::Nullable<System::DateTime> get(); }

        property System::String^ Name { System::String^ get(); }

        property System::String^ Comment { System::String^ get(); }

        property System::String^ Creator { System::String^ get(); }

        // TODO: nodes_t const& nodes () const;
        // TODO: void add_node(std::pair<std::string, int> const& node);
        // TODO: bool parse_info_section(lazy_entry const& e, error_code& ec, int flags);
        // TODO: lazy_entry const* info(char const* key) const;
        // TODO: void swap(torrent_info& ti);

        property int MetadataSize { int get(); }

        // TODO:    boost::shared_array<char> metadata () const;

        property bool IsMerkleTorrent { bool get(); }
    };
}
