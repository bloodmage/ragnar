#pragma once
namespace libtorrent
{
	typedef unsigned char byte;
}
using libtorrent::byte;
namespace Ragnar
{

    ref class AddTorrentParams;
    interface class IAlertFactory;
    enum class SessionAlertCategory : unsigned int;
    ref class SessionSettings;
    ref class SessionStatus;
    ref class TorrentHandle;
	ref class PeSettings;

    public interface class ISession
    {
        void LoadState(cli::array<byte>^ buffer);
        cli::array<byte>^ SaveState();

        void PostTorrentUpdates();

        TorrentHandle^ FindTorrent(System::String^ infoHash);
        System::Collections::Generic::IEnumerable<TorrentHandle^>^ GetTorrents();

        TorrentHandle^ AddTorrent(AddTorrentParams^ params);

        void AsyncAddTorrent(AddTorrentParams^ params);

        void Pause();
        void Resume();

        property bool IsPaused { bool get(); }

        SessionStatus^ QueryStatus();

        property bool IsDhtRunning { bool get(); }

        void StartDht();

        void StopDht();

        void SetKey(int key);

        int ListenOn(int lower, int upper);

		int ListenOn(int lower, int upper, System::Net::IPAddress^ ip);

        property bool IsListening { bool get(); }

        property int ListenPort { int get(); }

        property int SslListenPort { int get(); }

        void RemoveTorrent(TorrentHandle^ handle);
        
        void RemoveTorrent(TorrentHandle^ handle, bool removeData);

        SessionSettings^ QuerySettings();

        void SetSettings(SessionSettings^ settings);

		PeSettings^ QueryPeerEncryptionSettings();

		void SetPeSettings(PeSettings^ settings);

        property IAlertFactory^ Alerts { IAlertFactory^ get(); }

        void SetAlertMask(SessionAlertCategory mask);

        void StopLsd();

        void StartLsd();

        void StopUpnp();

        void StartUpnp();

        void StopNatPmp();

        void StartNatPmp();
    };
}

