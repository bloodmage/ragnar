#include "stdafx.h"
#include "UtpStatus.h"

#include <libtorrent\session_status.hpp>

namespace Ragnar
{
	UtpStatus::UtpStatus(const libtorrent::utp_status &status)
	{
		this->_utp_status = new libtorrent::utp_status(status);
	}

	UtpStatus::~UtpStatus()
	{
		delete this->_utp_status;
	}

	int UtpStatus::NumIdle::get()
	{
		return this->_utp_status->num_idle;
	}

	int UtpStatus::NumSynSent::get()
	{
		return this->_utp_status->num_syn_sent;
	}
	int UtpStatus::NumConnected::get()
	{
		return this->_utp_status->num_connected;
	}

	int UtpStatus::NumFinSent::get()
	{
		return this->_utp_status->num_fin_sent;
	}
	int UtpStatus::NumCloseWait::get()
	{
		return this->_utp_status->num_close_wait;
	}
}