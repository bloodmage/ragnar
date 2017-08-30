#include "stdafx.h"
#include "PeSettings.h"

#include <libtorrent\session_settings.hpp>

namespace Ragnar
{
	PeSettings::PeSettings(const libtorrent::pe_settings &settings)
	{
		this->_pe_settings = new libtorrent::pe_settings(settings);
	}

	PeSettings::~PeSettings()
	{
		delete this->_pe_settings;
	}

	libtorrent::pe_settings* PeSettings::get_ptr()
	{
		return this->_pe_settings;
	}

	PeSettings_EncPolicy PeSettings::OutgoingEncryptionPolicy::get()
	{
		switch (this->_pe_settings->out_enc_policy)
		{
		case libtorrent::pe_settings::enc_policy::disabled:
			return PeSettings_EncPolicy::Disabled;
		case libtorrent::pe_settings::enc_policy::enabled:
			return PeSettings_EncPolicy::Enabled;
		case libtorrent::pe_settings::enc_policy::forced:
			return PeSettings_EncPolicy::Forced;
		default:
			break;
		}
	}

	void PeSettings::OutgoingEncryptionPolicy::set(PeSettings_EncPolicy value)
	{
		if (value.Equals(PeSettings_EncPolicy::Disabled))
		{
			this->_pe_settings->out_enc_policy = libtorrent::pe_settings::enc_policy::disabled;
		}
		else if (value.Equals(PeSettings_EncPolicy::Enabled))
		{
			this->_pe_settings->out_enc_policy = libtorrent::pe_settings::enc_policy::enabled;
		}
		else if (value.Equals(PeSettings_EncPolicy::Forced))
		{
			this->_pe_settings->out_enc_policy = libtorrent::pe_settings::enc_policy::forced;
		}
	}

	PeSettings_EncPolicy PeSettings::IncomingEncryptionPolicy::get()
	{
		switch (this->_pe_settings->in_enc_policy)
		{
		case libtorrent::pe_settings::enc_policy::disabled:
			return PeSettings_EncPolicy::Disabled;
		case libtorrent::pe_settings::enc_policy::enabled:
			return PeSettings_EncPolicy::Enabled;
		case libtorrent::pe_settings::enc_policy::forced:
			return PeSettings_EncPolicy::Forced;
		default:
			break;
		}
	}

	void PeSettings::IncomingEncryptionPolicy::set(PeSettings_EncPolicy value)
	{
		if (value.Equals(PeSettings_EncPolicy::Disabled))
		{
			this->_pe_settings->in_enc_policy = libtorrent::pe_settings::enc_policy::disabled;
		}
		else if (value.Equals(PeSettings_EncPolicy::Enabled))
		{
			this->_pe_settings->in_enc_policy = libtorrent::pe_settings::enc_policy::enabled;
		}
		else if (value.Equals(PeSettings_EncPolicy::Forced))
		{
			this->_pe_settings->in_enc_policy = libtorrent::pe_settings::enc_policy::forced;
		}
	}

	PeSettings_EncLevel PeSettings::AllowedEncryptionLevel::get()
	{
		switch (this->_pe_settings->allowed_enc_level)
		{
		case libtorrent::pe_settings::enc_level::both:
			return PeSettings_EncLevel::Both;
		case libtorrent::pe_settings::enc_level::plaintext:
			return PeSettings_EncLevel::Plaintext;
		case libtorrent::pe_settings::enc_level::rc4:
			return PeSettings_EncLevel::RC4;
		default:
			break;
		}
	}

	void PeSettings::AllowedEncryptionLevel::set(PeSettings_EncLevel value)
	{
		if (value.Equals(PeSettings_EncLevel::Both))
		{
			this->_pe_settings->allowed_enc_level = libtorrent::pe_settings::enc_level::both;
		}
		else if (value.Equals(PeSettings_EncLevel::Plaintext))
		{
			this->_pe_settings->allowed_enc_level = libtorrent::pe_settings::enc_level::plaintext;
		}
		else if (value.Equals(PeSettings_EncLevel::RC4))
		{
			this->_pe_settings->allowed_enc_level = libtorrent::pe_settings::enc_level::rc4;
		}
	}

	bool PeSettings::PreferRC4::get()
	{
		return this->_pe_settings->prefer_rc4;
	}

	void PeSettings::PreferRC4::set(bool value)
	{
		this->_pe_settings->prefer_rc4 = value;
	}
}