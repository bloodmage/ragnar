#pragma once

namespace Ragnar
{
	[System::FlagsAttribute]
	public enum class PeSettings_EncPolicy : unsigned char
	{
		// Only encrypted connections are allowed. Incoming connections that are not encrypted are closed and if the encrypted outgoing connection fails, a non-encrypted retry will not be made.
		Forced,
		// Encrypted connections are enabled, but non-encrypted connections are allowed. An incoming non-encrypted connection will be accepted, and if an outgoing encrypted connection fails, a non- encrypted connection will be tried.
		Enabled,
		// Only non-encrypted connections are allowed.
		Disabled
	};
}