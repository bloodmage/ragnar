#include "stdafx.h"
#include "AddTorrentParams.h"
#include "TorrentInfo.h"
#include "AlertFactory.h"
#include "Fingerprint.h"
#include "Session.h"
#include "SessionAlertCategory.h"
#include "SessionSettings.h"
#include "SessionStatus.h"
#include "TorrentHandle.h"
#include "PeSettings.h"
#include "Utils.h"
#include "SHA1Hash.h"
#include "DhtDynamicItemPutter.h"

#include "libtorrent/alloca.hpp"

#include "Plugin.h"

#include <libtorrent\bencode.hpp>
#include <libtorrent\lazy_entry.hpp>
#include <libtorrent\session.hpp>
#include <libtorrent\session_status.hpp>
#include <libtorrent\torrent_status.hpp>
#include <libtorrent\extensions.hpp>

using namespace System::Runtime::InteropServices;
namespace Ragnar
{
    Session::Session()
    {
        this->_session = new libtorrent::session();
        this->_alertFactory = gcnew AlertFactory(*this->_session);
		dhtputter = gcnew DhtDynamicPutter(this);
    }

    Session::Session(Fingerprint^ fingerprint)
    {
        this->_session = new libtorrent::session(*fingerprint->get_ptr());
        this->_alertFactory = gcnew AlertFactory(*this->_session);
		dhtputter = gcnew DhtDynamicPutter(this);
	}

    Session::~Session()
    {
        if (this->_disposed)
        {
            return;
        }

		delete this->_session;
		delete _alertFactory;
		dhtputter->Unload();
		delete dhtputter;
		this->_disposed = true;
    }

    Session::!Session()
    {
		this->~Session();
    }

    void Session::LoadState(cli::array<byte>^ buffer)
    {
        pin_ptr<unsigned char> ptr = &buffer[0];
        const char *pbegin = (const char*)(const unsigned char*)ptr;
        const char *pend = pbegin + buffer->Length;

        libtorrent::lazy_entry entry;
        libtorrent::lazy_bdecode(pbegin, pend, entry);

        this->_session->load_state(entry);
    }

    cli::array<byte>^ Session::SaveState()
    {
        libtorrent::entry entry;
        this->_session->save_state(entry);

        return Utils::GetByteArrayFromLibtorrentEntry(entry);
    }

    void Session::PostTorrentUpdates()
    {
        this->_session->post_torrent_updates();
    }

	TorrentHandle^ Session::FindTorrent(System::String^ infoHash)
	{
		auto sha1 = Utils::GetSha1HashFromString(infoHash);
		auto handle = this->_session->find_torrent(sha1);

		if (!handle.is_valid())
		{
			return nullptr;
		}

		return gcnew TorrentHandle(handle);
	}

	TorrentHandle^ Session::FindTorrent(SHA1Hash^ infoHash)
	{
		auto handle = this->_session->find_torrent(*infoHash->_hash);

		if (!handle.is_valid())
		{
			return nullptr;
		}

		return gcnew TorrentHandle(handle);
	}

	System::Collections::Generic::IEnumerable<TorrentHandle^>^ Session::GetTorrents()
	{
		auto iterator = this->_session->get_torrents();
		auto result = gcnew System::Collections::Generic::List<TorrentHandle^>(iterator.size());

		for (int i = 0; i < iterator.size(); i++)
		{
			result->Add(gcnew TorrentHandle(iterator[i]));
		}

		return result;
	}

	TorrentHandle^ Session::AddTorrent(AddTorrentParams^ params)
	{
		libtorrent::torrent_handle handle = this->_session->add_torrent(params->get_ptr());
		if (params->TorrentInfo != nullptr)
			params->TorrentInfo->_info = NULL;
		return gcnew TorrentHandle(handle);
	}

	void Session::AsyncAddTorrent(AddTorrentParams^ params)
	{
		this->_session->async_add_torrent(params->get_ptr());
		if (params->TorrentInfo != nullptr)
			params->TorrentInfo->_info = NULL;
	}

	void Session::Pause()
	{
		this->_session->pause();
	}

	void Session::Resume()
	{
		this->_session->resume();
	}

	bool Session::IsPaused::get()
	{
		return this->_session->is_paused();
	}

	SessionStatus^ Session::QueryStatus()
	{
		return gcnew SessionStatus(this->_session->status());
	}

	bool Session::IsDhtRunning::get()
	{
		return this->_session->is_dht_running();
	}

	void Session::StartDht()
	{
		this->_session->start_dht();
	}

	void Session::StopDht()
	{
		this->_session->stop_dht();
	}

	static std::string managedStrToNative(System::String^ sysstr)
	{
		using System::IntPtr;
		using System::Runtime::InteropServices::Marshal;

		IntPtr ip = Marshal::StringToHGlobalAnsi(sysstr);
		std::string outString = static_cast<const char*>(ip.ToPointer());
		Marshal::FreeHGlobal(ip);
		return outString;
	}

	void Session::AddDhtNode(System::String^ nodeip, int nodeport)
	{
		std::string localip = managedStrToNative(nodeip);
		this->_session->add_dht_node(std::make_pair(localip, nodeport));
	}

	void Session::DhtGetItem(SHA1Hash^ target)
	{
		this->_session->dht_get_item(*target->_hash);
	}

	void Session::DhtGetItem(array<byte, 1>^ key, array<byte, 1>^ salt)
	{
		if (key->Length != 32) throw gcnew InvalidOperationException("Key must be byte array of length 32");
		boost::array<char, 32> key_;
		Marshal::Copy(key, 0, System::IntPtr(key_.begin()), 32);

		std::string salt_;
		if (salt == nullptr || salt->Length == 0)
			salt_ = std::string();
		else
		{
			pin_ptr<byte>salta = &salt[0];
			salt_ = std::string((char*)salta, (char*)salta + salt->Length);
		}

		this->_session->dht_get_item(key_, salt_);
	}

	SHA1Hash^ Session::DhtPutItem(array<byte, 1>^ data, int start, int dlen)
	{
		if (start < 0 || start >= data->Length) throw gcnew InvalidOperationException("put item range error (invalid start)");
		if (dlen <= 0 || start + dlen > data->Length) throw gcnew InvalidOperationException("put item range error (invalid dlen)");
		pin_ptr<byte> data_ = &data[0];
		auto entry = libtorrent::bdecode((byte*)data_ + start, (byte*)data_ + start + dlen);
		return gcnew SHA1Hash(this->_session->dht_put_item(entry));
	}

	SHA1Hash^ Session::DhtPutItem(byte* data, int dlen)
	{
		auto entry = libtorrent::bdecode(data, data + dlen);
		return gcnew SHA1Hash(this->_session->dht_put_item(entry));
	}

	DhtDynamicPutter^ Session::DhtDynamicItemPutter::get()
	{
		return dhtputter;
	}

	void Session::AddTorrentExtension(Unsafe::Plugin^ plg)
	{
		plg->installed = true;
		plg->plugin->handle = plg;
		this->_session->add_extension(boost::shared_ptr<libtorrent::plugin>((libtorrent::plugin*)plg->plugin));
	}

    void Session::SetKey(int key)
    {
        this->_session->set_key(key);
    }

	int Session::ListenOn(int lower, int upper)
	{
		libtorrent::error_code err;
		this->_session->listen_on(std::make_pair(lower, upper), err);
		return err.value();
	}

	int Session::ListenOn(int lower, int upper, System::Net::IPAddress^ ip)
	{
		const char* interfa = Utils::GetStdStringFromManagedString(ip->ToString()).c_str();
		libtorrent::error_code err;
		this->_session->listen_on(std::make_pair(lower, upper), err, interfa);
		return err.value();
	}

    bool Session::IsListening::get()
    {
        return this->_session->is_listening();
    }

    int Session::ListenPort::get()
    {
        return this->_session->listen_port();
    }

    int Session::SslListenPort::get()
    {
        return this->_session->ssl_listen_port();
    }

    void Session::RemoveTorrent(TorrentHandle^ handle)
    {
        this->RemoveTorrent(handle, false);
    }

    void Session::RemoveTorrent(TorrentHandle^ handle, bool removeData)
    {
        this->_session->remove_torrent(*handle->get_ptr(), removeData ? libtorrent::session::delete_files : 0);
    }

    SessionSettings^ Session::QuerySettings()
    {
        return gcnew SessionSettings(this->_session->settings());
    }

    void Session::SetSettings(SessionSettings^ settings)
    {
        this->_session->set_settings(*settings->get_ptr());
    }

	PeSettings^ Session::QueryPeerEncryptionSettings()
	{
		return gcnew PeSettings(this->_session->get_pe_settings());
	}
	
	void Session::SetPeSettings(PeSettings^ pe_settings)
	{
		this->_session->set_pe_settings(*pe_settings->get_ptr());
	}

    IAlertFactory^ Session::Alerts::get()
    {
        return this->_alertFactory;
    }
    
    void Session::SetAlertMask(SessionAlertCategory mask)
    {
        this->_session->set_alert_mask((unsigned int) mask);
    }

    void Session::StopLsd()
    {
        this->_session->stop_lsd();
    }

    void Session::StartLsd()
    {
        this->_session->start_lsd();
    }

    void Session::StopUpnp()
    {
        this->_session->stop_upnp();
    }

    void Session::StartUpnp()
    {
        this->_session->start_upnp();
    }

    void Session::StopNatPmp()
    {
        this->_session->stop_natpmp();
    }

    void Session::StartNatPmp()
    {
        this->_session->start_natpmp();
    }
}
