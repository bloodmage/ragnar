#pragma once


namespace Ragnar
{
	public ref class ED25519Helper abstract sealed
	{
	public:
		///<summary>
		///Create seed of length 32, in binary form
		///</summary>
		static array<unsigned char, 1>^ CreateSeed();
		///<summary>
		///Create keypair (pubkey 32, privkey 64) from seed, all in binary form
		///</summary>
		static System::Tuple<array<unsigned char, 1>^, array<unsigned char, 1>^>^ CreateKeypair(array<unsigned char, 1>^ seed);
		///<summary>
		///Sign message to signature buffer (64), pubkey 32, privkey 64, all in binary form
		///</summary>
		static void Sign(array<unsigned char, 1>^ signature, array<unsigned char, 1>^ message, array<unsigned char, 1>^ pubkey, array<unsigned char, 1>^ privkey);
		///<summary>
		///Return: 1: ok, 0: fail
		///</summary>
		static int Verify(const unsigned char* signature, const unsigned char* message, size_t message_len, const unsigned char* public_key);

		static System::String^ ToHexString(array<unsigned char, 1>^ bytes);
		static array<unsigned char, 1>^ FromHexString(System::String^ hexdata);
	};
}