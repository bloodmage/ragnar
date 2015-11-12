#pragma once

namespace libtorrent
{
	struct utp_status;
}

namespace Ragnar
{
	public ref class UtpStatus
	{
	private:
		libtorrent::utp_status* _utp_status;
	internal:
		UtpStatus(const libtorrent::utp_status &utp_status);

	public:
		~UtpStatus();
		property int NumIdle { int get(); }
		property int NumSynSent { int get(); }
		property int NumConnected { int get(); }
		property int NumFinSent { int get(); }
		property int NumCloseWait { int get(); }
	};
}