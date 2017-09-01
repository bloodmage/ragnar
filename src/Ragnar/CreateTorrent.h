#pragma once
#include "FileStorage.h"
#include "Utils.h"
#include "Entry.h"
#include "SHA1Hash.h"

#include <libtorrent/create_torrent.hpp>

namespace Ragnar
{
	///<summary>
	///Reference following C++ sample procedure<br/>
	///<br/>
	///file_storage fs;<br/>
	///<br/>
	/// // recursively adds files in directories<br/>
	///add_files(fs, "./my_torrent");<br/>
	///<br/>
	///create_torrent t(fs);<br/>
	///t.add_tracker("http://my.tracker.com/announce");<br/>
	///t.set_creator("libtorrent example");<br/>
	///<br/>
	///// reads the files and calculates the hashes<br/>
	///set_piece_hashes(t, ".");<br/>
	///<br/>
	///	ofstream out("my_torrent.torrent", std::ios_base::binary);<br/>
	///bencode(std::ostream_iterator&lt;char&gt;(out), t.generate());<br/>
	///</summary>
	public ref class CreateTorrent
	{
	internal:
		libtorrent::create_torrent * ptr;
		FileStorage^ storage;
	public:
		CreateTorrent(FileStorage^ storage) { this->storage = storage; ptr = new libtorrent::create_torrent(*storage->stor); }
		~CreateTorrent() { if (ptr != NULL) { free(ptr); ptr = NULL; storage->~FileStorage(); } }
		!CreateTorrent() { this->~CreateTorrent(); }
		void AddTracker(System::String^ tracker, int tier) { this->ptr->add_tracker(Utils::GetStdStringFromManagedString(tracker), tier); }
		void AddTracker(System::String^ tracker) { this->ptr->add_tracker(Utils::GetStdStringFromManagedString(tracker), 0); }
		void SetCreator(System::String^ creator) {
			if (creator == nullptr) this->ptr->set_creator(NULL);
			else {
				auto o = Utils::GetStdStringFromManagedString(creator);
				this->ptr->set_creator(o.c_str());
			}
		}
		void SetComment(System::String^ comment) {
			if (comment == nullptr) this->ptr->set_comment(NULL);
			else {
				auto o = Utils::GetStdStringFromManagedString(comment);
				this->ptr->set_comment(o.c_str());
			}
		}
		void SetPieceHashes(System::String^ path) {
			auto hg = Utils::GetStdStringFromManagedString(path);
			libtorrent::set_piece_hashes(*ptr, hg);
		}

		Unsafe::Entry^ Generate() {
			Unsafe::Entry^ ent = Unsafe::Entry::Wrap(ptr->generate());
			return ent;
		}
		void AddUrlSeed(System::String^ url) {
			auto o = Utils::GetStdStringFromManagedString(url);
			ptr->add_url_seed(o);
		}
		void AddHTTPSeed(System::String^ url) {
			auto o = Utils::GetStdStringFromManagedString(url);
			ptr->add_http_seed(o);
		}
		void AddNode(System::String^ nodeip, int nodeport)
		{
			auto o = Utils::GetStdStringFromManagedString(nodeip);
			ptr->add_node(std::make_pair(o, nodeport));
		}

		property bool IsPriv{bool get() { return ptr->priv(); } void set(bool v){ ptr->set_priv(v); } }
		void AddSimilarTorrent(SHA1Hash^ ih) { ptr->add_similar_torrent(*ih->_hash); }
		void AddCollection(System::String^ c) {
			auto o = Utils::GetStdStringFromManagedString(c);
			ptr->add_collection(o);
		}

	};
}