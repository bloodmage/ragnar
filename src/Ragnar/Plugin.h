#pragma once
#include "SHA1Hash.h"
#include "ErrorCodeEnum.h"

#include "libtorrent/extensions.hpp"
#include <vcclr.h>

namespace Ragnar
{
	ref class TorrentHandle;
	namespace Unsafe
	{
		ref class Torrent;
		ref class Plugin;
		ref class TorrentPlugin;
		ref class PeerPlugin;
		ref class LazyEntry;
		class _pluginHandler;
		class _torrentPluginHandler;
		class _peerPluginHandler;

		public ref class Plugin
		{
		public:
			Plugin();

		internal:
			_pluginHandler*plugin;
			bool installed = false;
			~Plugin();
			!Plugin();
			void Unload();

		public:
			virtual uint32_t ImplementedFeatures() { return 0; }
			virtual TorrentPlugin^ NewTorrent(Ragnar::TorrentHandle^ torrent, void* userdata) { return nullptr; }
			virtual void Added(void* sessionimpl) { }
			virtual void RegisterDHTExtensions(void* ext) { }
			virtual void OnAlert(void const* alert) { }
			virtual void OnTick() { }
			virtual bool OnUnknownTorrent(Ragnar::SHA1Hash^ infohash, void* pc, void* p) { return false; }
			virtual bool OnOptimisticUnchoke(void* peers) { return false; }
			virtual void SaveState(void const* entry) { }
			virtual void LoadState(void const* lazyentry) { }

			virtual void OnDestroy() { }
		};

		class _pluginHandler : libtorrent::plugin
		{
		public:
			_pluginHandler() { }
			~_pluginHandler() { if ((Plugin^)handle != nullptr) handle->Unload(); }

			gcroot<Plugin^> handle;

			boost::uint32_t implemented_features() { return this->handle->ImplementedFeatures(); }
			boost::shared_ptr<libtorrent::torrent_plugin> new_torrent(libtorrent::torrent_handle const& torrent, void* userdata);
			void added(libtorrent::session_handle*handle) { this->handle->Added(handle); }
			void register_dht_extensions(libtorrent::dht_extensions_t&t) { handle->RegisterDHTExtensions(&t); }
			void on_alert(libtorrent::alert const*alert) { handle->OnAlert(alert); }
			bool OnUnknownTorrent(libtorrent::sha1_hash const& info_hash, libtorrent::peer_connection_handle const& pc, libtorrent::add_torrent_params& p) { return handle->OnUnknownTorrent(gcnew SHA1Hash(info_hash), (void*)&pc, &p); }
			void on_tick() { handle->OnTick(); }
			bool on_optimistic_unchoke(std::vector<libtorrent::peer_connection_handle>& peers) { return handle->OnOptimisticUnchoke(&peers); }
			void save_state(libtorrent::entry& entry) const { handle->SaveState(&entry); }
			void load_state(libtorrent::bdecode_node const& entry) { handle->LoadState(&entry); }
		};

		public enum class PairFlags : int
		{
			Firsttime = 1,
			Filtered = 2
		};

		ref class PeerConnectionHandle;

		public ref class TorrentPlugin
		{
		public:
			TorrentPlugin();

		internal:
			_torrentPluginHandler* plugin;
			bool installed = false;
			~TorrentPlugin();
			!TorrentPlugin();
			void Unload();

		public:
			virtual PeerPlugin^ NewConnection(PeerConnectionHandle^ peerconn) { return nullptr; }
			virtual void OnPiecePass(int idx) { }
			virtual void OnPieceFailed(int idx) { }
			virtual void Tick() { }
			virtual bool OnPause() { return false; }
			virtual bool OnResume() { return false; }
			virtual void OnFilesChecked() { }
			virtual void OnState(int state) { }
			virtual void OnUnload() { }
			virtual void OnLoad() { }

			virtual void OnAddPeer(System::Net::IPEndPoint^ endpoint, int src, PairFlags flags) { }

			virtual void OnDestroy() { }
		};

		ref class Entry;
		public value class PeerRequest
		{
		public:
			PeerRequest(libtorrent::peer_request const& r);
			int Piece;
			int Start;
			int Length;
		};

		public ref class PeerPlugin
		{
		public:
			PeerPlugin();

		private:

		internal:
			_peerPluginHandler* plugin;
			char* typebuf;
			bool installed = false;
			~PeerPlugin();
			!PeerPlugin();
			void Unload();

		public:
			virtual System::String^ Type() { return ""; }
			virtual void AddHandshake(Entry^ entry) {}
			virtual void OnDisconnect(const void* errcode) {}
			virtual void OnConnected() {}
			///<summary>
			///Will drop peerplugin when return false
			///</summary>
			virtual bool OnHandshake(char const* reserved_bits){ return true; }
			virtual bool OnExtensionHandshake(LazyEntry^ lazyentry) { return true; }

			virtual bool OnChoke() { return false; }
			virtual bool OnUnchoke() { return false; }
			virtual bool OnInterested() { return false; }
			virtual bool OnNotInterested() { return false; }
			virtual bool OnHave(int index) { return false; }
			virtual bool OnDontHave(int index) { return false; }
			virtual bool OnBitfield(const void* bitfield) { return false; }
			virtual bool OnHaveAll() { return false; }
			virtual bool OnHaveNone() { return false; }
			virtual bool OnAllowedFast(int index) { return false; }
			virtual bool OnRequest(PeerRequest request) { return false; }
			virtual bool OnPiece(PeerRequest piece, void*data) { return false; }
			virtual bool OnCancel(PeerRequest request) { return false; }
			virtual bool OnReject(PeerRequest request) { return false; }
			virtual bool OnSuggest(int index) { return false; }

			virtual void SentUnchoke(){}
			virtual void SentPayload(int bytes){}
			virtual bool CanDisconnect(const void* errcode) { return true; }

			virtual bool OnExtended(int length, int msg, char const* buf, int buflen) { return false; }
			virtual bool OnUnknownMessage(int length, int msg, char const* buf, int buflen){ return false; }
			virtual void OnPiecePass(int index) {}
			virtual void OnPieceFailed(int index) {}
			virtual void Tick() {}
			virtual bool WriteRequest(PeerRequest request) { return false; }

			virtual void OnDestroy() { }
		};

	}

}