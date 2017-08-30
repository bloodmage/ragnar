#include "stdafx.h"
#include "Plugin.h"
#include "Entry.h"
#include "LazyEntry.h"
#include "PeerConnectionHandle.h"
#include "TorrentHandle.h"

#include "libtorrent/extensions.hpp"
#include "libtorrent/torrent.hpp"
#include "libtorrent/aux_/session_impl.hpp"
#include "Utils.h"
#include <vcclr.h>

using namespace System::Runtime::InteropServices;
namespace Ragnar
{
	namespace Unsafe
	{

		Plugin::Plugin()
		{
			plugin = new _pluginHandler();
		}

		Plugin::~Plugin()
		{
			if (plugin != NULL && !installed)
			{
				delete plugin;
				OnDestroy();
				plugin = NULL;
			}
		}

		Plugin::!Plugin()
		{
			this->~Plugin();
		}

		void Plugin::Unload()
		{
			plugin = NULL;
			OnDestroy();
		}

		class _torrentPluginHandler : libtorrent::torrent_plugin
		{
		public:
			_torrentPluginHandler() { }
			~_torrentPluginHandler() { if ((TorrentPlugin^)handle != nullptr) handle->Unload(); }

			gcroot<TorrentPlugin^> handle;

			boost::shared_ptr<libtorrent::peer_plugin> new_connection(libtorrent::peer_connection_handle const& conn);
			void on_piece_pass(int index) { handle->OnPiecePass(index); }
			void on_piece_failed(int index) { handle->OnPieceFailed(index); }
			void tick() { handle->Tick(); }
			bool on_pause() { return handle->OnPause(); }
			bool on_resume() { return handle->OnResume(); }
			void on_files_checked() { handle->OnFilesChecked(); }
			void on_unload() { handle->OnUnload(); }
			void on_load() { handle->OnLoad(); }
			void on_state(int s) { handle->OnState(s); }
			void on_add_peer(libtorrent::tcp::endpoint const& endp, int src, int flags) {
				std::string addrs = endp.address().to_string();
				System::Net::IPEndPoint^ mendp = gcnew System::Net::IPEndPoint(System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(addrs)), endp.port());
				handle->OnAddPeer(mendp, src, PairFlags(flags));
			}
		};


		TorrentPlugin::TorrentPlugin()
		{
			plugin = new _torrentPluginHandler();
		}

		TorrentPlugin::~TorrentPlugin()
		{
			if (plugin != NULL && !installed)
			{
				delete plugin;
				OnDestroy();
				plugin = NULL;
			}
		}

		TorrentPlugin::!TorrentPlugin()
		{
			this->~TorrentPlugin();
		}

		void TorrentPlugin::Unload()
		{
			plugin = NULL;
			OnDestroy();
		}

		PeerRequest::PeerRequest(libtorrent::peer_request const& r)
		{
			Piece = r.piece;
			Start = r.start;
			Length = r.length;
		}

		class _peerPluginHandler : libtorrent::peer_plugin
		{
		public:
			_peerPluginHandler() { }
			~_peerPluginHandler() { if ((PeerPlugin^)handle != nullptr) handle->Unload(); }

			gcroot<PeerPlugin^> handle;

			char* handlerType;
			virtual char const* type() const { return handlerType; }
			virtual void add_handshake(libtorrent::entry& entry) { handle->AddHandshake(Entry::Wrap(&entry)); }
			virtual void on_disconnect(libtorrent::error_code const& ec) { handle->OnDisconnect(&ec); }
			virtual void on_connected() { handle->OnConnected(); }
			virtual bool on_handshake(char const* reserved_bits) { return handle->OnHandshake(reserved_bits); }
			virtual bool on_extension_handshake(libtorrent::bdecode_node const& lazyentry) { return handle->OnExtensionHandshake(LazyEntry::Wrap((libtorrent::lazy_entry*)&lazyentry)); }
			virtual bool on_choke() { return handle->OnChoke(); }
			virtual bool on_unchoke() { return handle->OnUnchoke(); }
			virtual bool on_interested() { return handle->OnInterested(); }
			virtual bool on_not_interested() { return handle->OnNotInterested(); }
			virtual bool on_have(int index) { return handle->OnHave(index); }
			virtual bool on_dont_have(int index) { return handle->OnDontHave(index); }
			virtual bool on_bitfield(libtorrent::bitfield const& bitfield) { return handle->OnBitfield(&bitfield); }
			virtual bool on_have_all() { return handle->OnHaveAll(); }
			virtual bool on_have_none() { return handle->OnHaveNone(); }
			virtual bool on_allowed_fast(int index) { return handle->OnAllowedFast(index); }
			virtual bool on_request(libtorrent::peer_request const& request) { return handle->OnRequest(PeerRequest(request)); }
			virtual bool on_piece(libtorrent::peer_request const& piece, libtorrent::disk_buffer_holder& data) { return handle->OnPiece(PeerRequest(piece), &data); }
			virtual bool on_cancel(libtorrent::peer_request const& request) { return handle->OnCancel(PeerRequest(request)); }
			virtual bool on_reject(libtorrent::peer_request const& request) { return handle->OnReject(PeerRequest(request)); }
			virtual bool on_suggest(int index) { return handle->OnSuggest(index); }
			virtual void sent_unchoke() { handle->SentUnchoke(); }
			virtual void sent_payload(int bytes) { handle->SentPayload(bytes); }
			virtual bool can_disconnect(libtorrent::error_code const& ec) { return handle->CanDisconnect(&ec); }
			virtual bool on_extended(int length, int msg, libtorrent::buffer::const_interval body) {
				return handle->OnExtended(length, msg, body.begin, body.end - body.begin);
			}
			virtual bool on_unknown_message(int length, int msg, libtorrent::buffer::const_interval body) {
				return handle->OnUnknownMessage(length, msg, body.begin, body.end - body.begin);
			}
			virtual void on_piece_pass(int index) { handle->OnPiecePass(index); }
			virtual void on_piece_failed(int index) { handle->OnPieceFailed(index); }
			virtual void tick() { handle->Tick(); }
			virtual bool write_request(libtorrent::peer_request const& request) { return handle->WriteRequest(PeerRequest(request)); }
		};

		PeerPlugin::PeerPlugin()
		{
			plugin = new _peerPluginHandler();
			auto pstr = Type();
			typebuf = (char*)Marshal::StringToHGlobalAnsi(pstr).ToPointer();
			plugin->handlerType = typebuf;
		}

		PeerPlugin::~PeerPlugin()
		{
			if (plugin != NULL && !installed)
			{
				delete plugin;
				Marshal::FreeHGlobal(System::IntPtr(typebuf));
				plugin = NULL;
				OnDestroy();
			}
		}

		PeerPlugin::!PeerPlugin()
		{
			this->~PeerPlugin();
		}


		void PeerPlugin::Unload()
		{
			plugin = NULL;
			Marshal::FreeHGlobal(System::IntPtr(typebuf));
			OnDestroy();
		}

		boost::shared_ptr<libtorrent::torrent_plugin> _pluginHandler::new_torrent(libtorrent::torrent_handle const& torrent, void* userdata)
		{
			auto dret = handle->NewTorrent(gcnew Ragnar::TorrentHandle(torrent), userdata);
			if (dret == nullptr)
				return boost::shared_ptr<libtorrent::torrent_plugin>();
			dret->installed = true;
			dret->plugin->handle = dret;
			return boost::shared_ptr<libtorrent::torrent_plugin>((libtorrent::torrent_plugin*)(dret->plugin));
		}

		boost::shared_ptr<libtorrent::peer_plugin> _torrentPluginHandler::new_connection(libtorrent::peer_connection_handle const& conn)
		{
			auto dret = handle->NewConnection(gcnew PeerConnectionHandle((libtorrent::peer_connection_handle*)&conn));
			if (dret == nullptr)
				return boost::shared_ptr<libtorrent::peer_plugin>();
			dret->installed = true;
			dret->plugin->handle = dret;
			return boost::shared_ptr<libtorrent::peer_plugin>((libtorrent::peer_plugin*)(dret->plugin));
		}
	}
}