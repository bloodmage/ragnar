#pragma once

namespace Ragnar
{
	[System::FlagsAttribute]
	public enum class PeSettings_EncLevel : unsigned char
	{
		// Only the handshake is encrypted, the bulk of the traffic remains unchanged.
		Plaintext = 1,
		// The entire stream is encrypted with RC4
		RC4 = 2,
		// Both RC4 and plaintext connections are allowed.
		Both =3
	};
}