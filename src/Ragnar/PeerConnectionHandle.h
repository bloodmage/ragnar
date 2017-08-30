#pragma once
#define TORRENT_DISABLE_ENCRYPTION
#include "SHA1Hash.h"
#include "PeerInfo.h"
#include "Utils.h"
#include "BitField.h"
#include "Torrent.h"
#include "Operations.h"

#include "libtorrent\peer_connection_handle.hpp"
#include "libtorrent\peer_connection.hpp"
#include "libtorrent\bt_peer_connection.hpp"
namespace libtorrent
{
	struct torrent_peer;
}
namespace Ragnar
{
	namespace Unsafe
	{
		public enum class ConnectionType : int
		{
			BITTORRENT_CONNECTION = 0,
			URL_SEED_CONNECTION = 1,
			HTTP_SEED_CONNECTION = 2
		};
		public enum class PeerSpeedEnum : int
		{
			SLOW = 1,
			MEDIUM = 2,
			FAST = 3
		};
		public enum class Sync
		{
			READ_ASYNC,
			READ_SYNC
		};

		public enum class MessageType : int
		{
			MSG_CHOKE = 0,
			MSG_UNCHOKE,
			MSG_INTERESTED,
			MSG_NOT_INTERESTED,
			MSG_HAVE,
			MSG_BITFIELD,
			MSG_REQUEST,
			MSG_PIECE,
			MSG_CANCEL,
			MSG_DHT_PORT,
			MSG_SUGGEST_PIECE,
			MSG_HAVE_ALL,
			MSG_HAVE_NONE,
			MSG_REJECT_REQUEST,
			MSG_ALLOWED_FAST,
			MSG_EXTENDED = 20,
			NUM_SUPPORTED_MESSAGES
		};

		public enum class ExtensionMessageType : int
		{
			EXT_HANDSHAKE = 0,
			EXT_UT_PEX = 1,
			EXT_UT_METADATA = 2,
			EXT_UT_HOLEPUNCH = 4,
			EXT_UPLOAD_ONLY = 3,
			EXT_SHARE_MODE = 8,
			EXT_LT_DONTHAVE = 7,
		};

		//<summary>
		//Thin wrapper around connection object, use with caution
		//</summary>
		public ref class PeerConnection
		{
		internal:
			libtorrent::peer_connection* conn;
			PeerConnection(libtorrent::peer_connection* conn) { this->conn = conn; }
		public:

			bool btpeer_PacketFinished() { return ((libtorrent::bt_peer_connection*)conn)->packet_finished(); }
			bool btpeer_SupportExtensions() { return ((libtorrent::bt_peer_connection*)conn)->support_extensions(); }
			//bool btpeer_SupportsEncryption() { return ((libtorrent::bt_peer_connection*)conn)->supports_encryption(); }


			property ConnectionType Type{ConnectionType get() { return ConnectionType(conn->type()); }}
			void Start(){ conn->start(); }
			//TODO: set_peer_info

			property void* PeerInfo { void* get(){ return conn->peer_info_struct(); } void set(void*value) { conn->set_peer_info((libtorrent::torrent_peer*)value); } }
			void PeerExceedsLimit(){ conn->peer_exceeds_limit(); }
			void PeerDisconnectedOther(){ conn->peer_disconnected_other(); }
			//PeerSpeedEnum PeerSpeed(){ return PeerSpeedEnum(conn->peer_speed()); }

			//property int UploadLimit{int get(){ return conn->get_upload_limit(); } void set(int value){ conn->set_upload_limit(value); } }
			//property int DownloadLimit{int get(){ return conn->get_download_limit(); } void set(int value){ conn->set_download_limit(value); } }

			//property bool IsPreferWholePieces{bool get(){ return conn->prefer_whole_pieces() == 1; } void set(bool value){ conn->prefer_whole_pieces(value ? 1 : 0); } }
			void SendAllowedSet() { conn->send_allowed_set(); }
			void Init() { conn->init(); }
			void OnMetadata() { conn->on_metadata(); }
			property int PickerOptions {int get(){ return conn->picker_options(); } void set(int o) { conn->picker_options(o); }}
			property int PreferContiguousBlocks {int get(){ return conn->prefer_contiguous_blocks(); } void set(int num){ conn->prefer_contiguous_blocks(num); } }

			property bool IsOnParole{bool get(){ return conn->on_parole(); }}

			property bool IsRequestLargeBlocks{bool get(){ return conn->request_large_blocks(); } void set(bool value){ conn->request_large_blocks(value); } }

			property bool IsEndGame{bool get(){ return conn->endgame(); } void set(bool value){ conn->set_endgame(value); }}
			property bool IsNoDownload{bool get(){ return conn->no_download(); } void set(bool value){ conn->no_download(value); }}
			property bool IsIgnoreStats{bool get(){ return conn->ignore_stats(); } void set(bool value){ conn->ignore_stats(value); }}
			//void SetPriority(int p){ conn->set_priority(p); }
			property uint32_t PeerRank{uint32_t get(){ return conn->peer_rank(); }}
			property bool FastReconnect{bool get(){ return conn->fast_reconnect(); } void set(bool value){ conn->fast_reconnect(value); }}

			void ReceivedPiece(int idx){ conn->received_piece(idx); }
			void AnnouncePiece(int idx){ conn->announce_piece(idx); }
			void SuperseedPiece(int replace_piece, int new_piece){ conn->superseed_piece(replace_piece, new_piece); }
			bool SuperSeededPiece(int index) { return conn->super_seeded_piece(index); }

			property bool CanWrite{bool get(){ return conn->can_write(); }}
			property bool CanRead{bool get(){ return conn->can_read(); }}
			//bool CanRead2(uint8_t* state){ return conn->can_read(state); }
			property bool IsSeed{bool get(){ return conn->is_seed(); }}

			property int NumHavePieces{int get(){ return conn->num_have_pieces(); }}
			property bool ShareMode{bool get(){ return conn->share_mode(); }void set(bool value){ conn->set_share_mode(value); }}
			property bool UploadOnly{bool get(){ return conn->upload_only(); }void set(bool value){ conn->set_upload_only(value); }}

			void SetHolepunchMode(){ conn->set_holepunch_mode(); }
			void KeepAlive(){ conn->keep_alive(); }

			property SHA1Hash^ Pid{SHA1Hash^ get(){ return gcnew SHA1Hash(conn->pid()); } void set(SHA1Hash^ hash){ conn->set_pid(*hash->_hash); }}
			bool HasPiece(int i){ return conn->has_piece(i); }

			//TODO: download_queue
			//TODO: request_queue
			//TODO: upload_queue
			void ClearRequestQueue(){ conn->clear_request_queue(); }

			//TOOD: download_queue_time

			property bool IsInteresting{bool get(){ return conn->is_interesting(); }}
			property bool IsChoked{bool get(){ return conn->is_choked(); }}
			property bool IsPeerInterested{bool get(){ return conn->is_peer_interested(); }}
			property bool HasPeerChoked{bool get(){ return conn->has_peer_choked(); }}

			void ChokeThisPeer(){ conn->choke_this_peer(); }
			void MaybeUnchokeThisPeer(){ conn->maybe_unchoke_this_peer(); }
			void UpdateInterest() { conn->update_interest(); }
			
			//TODO: get_peer_info
			//TODO: associated_torrent
			Torrent^ AssociatedTorrent() { return gcnew Torrent((libtorrent::torrent*)&conn->associated_torrent()); }
			bool IsAssociatedTorrentExpired() { return conn->associated_torrent().expired(); }

			//TODO: statistics
			//TODO: add_stat

			void SentBytes(int bytes_payload, int bytes_protocol) { conn->sent_bytes(bytes_payload, bytes_protocol); }
			void ReceivedBytes(int bytes_payload, int bytes_protocol){ conn->received_bytes(bytes_payload, bytes_protocol); }
			void TrancieveIpPacket(int bytes, bool ipv6) { conn->trancieve_ip_packet(bytes, ipv6); }
			void SentSyn(bool ipv6){ conn->sent_syn(ipv6); }
			void ReceivedSynack(bool ipv6){ conn->received_synack(ipv6); }

			//second_tick should be called by main_loop

			//void TimeoutRequests(){ conn->timeout_requests(); }
			
			//TODO: get_socket
			System::Net::IPEndPoint^ Remote()
			{
				const libtorrent::tcp::endpoint&r(conn->remote());
				return gcnew System::Net::IPEndPoint(System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(r.address().to_string())), r.port());
			}
			System::Net::IPEndPoint^ Local()
			{
				const libtorrent::tcp::endpoint&r(conn->local_endpoint());
				return gcnew System::Net::IPEndPoint(System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(r.address().to_string())), r.port());
			}
			///<summary>
			///With "const" prefix
			///</summary>
			BitField^ GetBitfield(){ return gcnew BitField((libtorrent::bitfield*)(void*)&conn->get_bitfield()); }

			//TODO: allowed_fast
			//TODO: suggested_pieces
			//TODO: connected_time
			//TODO: last_received
			//TODO: disconnect
			void Disconnect(int errenum, operation_t op, int errcode) { conn->disconnect(libtorrent::error_code(errenum, libtorrent::get_libtorrent_category()), (libtorrent::operation_t)(int)op, errcode); }

			void ConnectFailed(int errenum){ conn->connect_failed(libtorrent::error_code(errenum, libtorrent::get_libtorrent_category())); }

			//TODO connect_failed
			property bool IsDisconnecting{ bool get(){ return conn->is_disconnecting(); } }
			//TODO: on_connection_complete
			property bool IsConnecting{ bool get(){ return conn->is_connecting(); } }
			//property bool IsQueued{bool get(){ return conn->is_queued(); }}
			//void ResetUploadQuota(){ conn->reset_upload_quota(); }

			void ReceivedValidData(int index){ conn->received_valid_data(index); }
			bool ReceivedInvalidData(int index, bool single_peer){ return conn->received_invalid_data(index, single_peer); }

			property bool IsOutgoing{bool get(){ return conn->is_outgoing(); }}
			//TODO: received_listen_port

			property bool OnLocalNetwork{bool get(){ return conn->on_local_network(); }}
			///property bool IgnoreBandwidthLimits{bool get(){ return conn->ignore_bandwidth_limits(); }void set(bool value){ conn->ignore_bandwidth_limits(value); }}
			property bool IgnoreUnchokeSlots{bool get(){ return conn->ignore_unchoke_slots(); }}

			property bool Failed{bool get(){ return conn->failed(); }}

			property int DesiredQueueSize{int get(){ return conn->desired_queue_size(); }}
			property int DownloadPayloadRate{ int get(){ return conn->download_payload_rate(); } }
			void ResetChokeCounters(){ conn->reset_choke_counters(); }
			void DisconnectIfRedundant(){ conn->disconnect_if_redundant(); }

			void increase_est_reciprocation_rate() { conn->increase_est_reciprocation_rate(); }
			void decrease_est_reciprocation_rate() { conn->decrease_est_reciprocation_rate(); }
			int est_reciprocation_rate() { return conn->est_reciprocation_rate(); }

			//TODO: incoming*
			//TODO: start_receive_piece
			//TODO: can_disconnect
			property bool HasMetadata{bool get(){ return conn->has_metadata(); } void set(bool value){ conn->set_has_metadata(value); }}

			bool send_choke() { return conn->send_choke(); }
			bool send_unchoke() { return conn->send_unchoke(); }
			void send_interested() { conn->send_interested(); }
			void send_not_interested() { conn->send_not_interested(); }
			void send_suggest(int piece) { conn->send_suggest(piece); }

			void SnubPeer(){ conn->snub_peer(); }

			property bool CanRequestTimeCritical{bool get(){ return conn->can_request_time_critical(); }}
			//TODO: make_time_critical
			//TODO: add_request

			void CancelAllRequests(){ conn->cancel_all_requests(); }
			//TODO: cancel_request

			void SendBlockRequests(){ conn->send_block_requests(); }
			//int BandwidthThrottle(int channel){ return conn->bandwidth_throttle(channel); }
			void AssignBandwidth(int channel, int amount){ conn->assign_bandwidth(channel, amount); }

			bool InHandshake(){ return conn->in_handshake(); }

			//TODO: downloading_piece_progress

			//TODO: sendbuffer with fun and userdata
			void SendBuffer(char* begin, int size, bool isrequest) { conn->send_buffer(begin, size, isrequest ? libtorrent::peer_connection::message_type_request : 0); }
			void SendBuffer(char* begin, int size) { conn->send_buffer(begin, size, 0); }

			void SetupSend(){ conn->setup_send(); }

			void CorkSocket(){ conn->cork_socket(); }
			void UncorkSocket(){ conn->uncork_socket(); }
			property bool IsCorked{ bool get(){ return conn->is_corked(); } }
			//TODO: append_send_buffer

			///<summary>
			///data in ptr of buffer must not change
			///</summary>
			void AppendConstSendBuffer(char* buffer, int size){ conn->append_const_send_buffer(buffer, size); }

			//TODO: set_country has_country

			property int OutstandingBytes{int get(){ return conn->outstanding_bytes(); }}
			property int SendBufferSize{int get(){ return conn->send_buffer_size(); }}
			property int SendBufferCapacity{int get(){ return conn->send_buffer_capacity(); }}
			//property int PacketSize{int get(){ return conn->packet_size(); }}
			/*bool btpeer_PacketFinished() { return ((libtorrent::bt_peer_connection*)conn)->packet_finished(); }
			bool btpeer_SupportExtensions() { return ((libtorrent::bt_peer_connection*)conn)->support_extensions(); }
			bool btpeer_SupportsEncryption() { return ((libtorrent::bt_peer_connection*)conn)->supports_encryption(); }*/
			//property int ReceivePos{int get(){ return conn->receive_pos(); }}

			property int MaxOutRequestQueue{int get(){ return conn->max_out_request_queue(); }void set(int value){ conn->max_out_request_queue(value); }}
			//TODO: last_seen_complete
			//TODO: uploaded_in_last_round, downloaded_in_last_round, uploaded_since_unchoked
			//TODO: time_of_last_unchoke
			void OnDisk(){ conn->on_disk(); }
			property int NumReadingBytes{int get(){ return conn->num_reading_bytes(); }}
			void SetupReceive(){ conn->setup_receive(); }

			int GetPriority(int channel){ return conn->get_priority(channel); }
		};

		public ref class PeerConnectionHandle
		{
		internal:
			libtorrent::peer_connection_handle* handle;
		public:
			PeerConnectionHandle(libtorrent::peer_connection_handle* handle){ this->handle = handle; }
			property ConnectionType Type{ConnectionType get() { return ConnectionType(handle->type()); }}

			property bool IsSeed { bool get(){ return handle->is_seed(); } }
			property bool UploadOnly{bool get(){ return handle->upload_only(); } }
			property SHA1Hash^ Pid{SHA1Hash^ get() { return gcnew SHA1Hash(handle->pid()); } }
			bool HasPiece(int i){ return handle->has_piece(i); }

			property bool IsInteresting{bool get(){ return handle->is_interesting(); }}
			property bool IsChoked{bool get(){ return handle->is_choked(); }}
			property bool IsPeerInterested{bool get(){ return handle->is_peer_interested(); }}
			property bool HasPeerChoked{bool get(){ return handle->has_peer_choked(); }}

			void ChokeThisPeer(){ handle->choke_this_peer(); }
			void MaybeUnchokeThisPeer(){ handle->maybe_unchoke_this_peer(); }

			//TODO: get_peer_info
			//TODO: associated_torrent
			System::Net::IPEndPoint^ Remote()
			{
				const libtorrent::tcp::endpoint&r(handle->remote());
				return gcnew System::Net::IPEndPoint(System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(r.address().to_string())), r.port());
			}
			System::Net::IPEndPoint^ Local()
			{
				const libtorrent::tcp::endpoint&r(handle->local_endpoint());
				return gcnew System::Net::IPEndPoint(System::Net::IPAddress::Parse(Utils::GetManagedStringFromStandardString(r.address().to_string())), r.port());
			}

			void Disconnect(int errenum, operation_t op, int errcode) { handle->disconnect(libtorrent::error_code(errenum, libtorrent::get_libtorrent_category()), (libtorrent::operation_t)(int)op, errcode); }
			property bool IsDisconnecting{ bool get(){ return handle->is_disconnecting(); } }
			property bool IsConnecting{ bool get(){ return handle->is_connecting(); } }
			property bool IsOutgoing{ bool get(){ return handle->is_outgoing(); } }
			property bool OnLocalNetwork{bool get(){ return handle->on_local_network(); }}
			property bool IgnoreUnchokeSlots{bool get(){ return handle->ignore_unchoke_slots(); }}
			property bool Failed{bool get(){ return handle->failed(); }}
			//TODO: can_disconnect
			property bool HasMetadata{bool get(){ return handle->has_metadata(); } }

			bool InHandshake(){ return handle->in_handshake(); }

			//TODO: downloading_piece_progress

			//TODO: sendbuffer with fun and userdata
			void SendBuffer(char* begin, int size, bool isrequest) { handle->send_buffer(begin, size, isrequest ? libtorrent::peer_connection::message_type_request : 0); }
			void SendBuffer(char* begin, int size) { handle->send_buffer(begin, size, 0); }

			bool btpeer_PacketFinished() { return ((libtorrent::bt_peer_connection_handle*)handle)->packet_finished(); }
			bool btpeer_SupportExtensions() { return ((libtorrent::bt_peer_connection_handle*)handle)->support_extensions(); }
			//bool btpeer_SupportsEncryption() { return ((libtorrent::bt_peer_connection_handle*)handle)->supports_encryption(); }

			PeerConnection^ GetPeerConnection() { return gcnew PeerConnection(handle->native_handle().get()); }
		};

	}
}