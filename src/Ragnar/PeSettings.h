#pragma once
#include "PeSettings_EncPolicy.h"
#include "PeSettings_EncLevel.h"

namespace libtorrent
{
	struct pe_settings;
}

namespace Ragnar
{
	//The PeSettings class is used to control the settings related to peer protocol encryption:
	public ref class PeSettings
	{
	private:
		libtorrent::pe_settings* _pe_settings;

	internal:
		PeSettings(const libtorrent::pe_settings &pe_settings);
		libtorrent::pe_settings* get_ptr();

	public:
		~PeSettings();

		property PeSettings_EncPolicy^ OutgoingEncryptionPolicy { PeSettings_EncPolicy^ get(); void set(PeSettings_EncPolicy^); }
		
		property PeSettings_EncPolicy^ IncomingEncryptionPolicy { PeSettings_EncPolicy^ get(); void set(PeSettings_EncPolicy^); }
		
		property PeSettings_EncLevel^ AllowedEncryptionLevel { PeSettings_EncLevel^ get(); void set(PeSettings_EncLevel^); }
		
		property bool PreferRC4 { bool get(); void set(bool); }
	};
}