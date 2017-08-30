#pragma once
#include "ISession.h"

namespace Ragnar
{
	///<summary>
	/// when put new item, entry should set to new entry, seq should be set to new sequence number, sign should be filled by SignMutableItem. Return true to start putting
	///</summary>
	public delegate bool PutterDelegate(array<byte, 1>^ key, array<byte, 1>^% entry, array<byte, 1>^ signature, System::UInt64% seq, array<byte, 1>^ salt);

	ref class Session;
	class _dhtputterhelper;

	public ref class DhtDynamicPutter sealed
	{
	private:
		_dhtputterhelper* helper;
	internal:
		Session^ host;
		System::Collections::Generic::List<System::Tuple<array<byte, 1>^, array<byte, 1>^, PutterDelegate^>^>^ delegates;
		void Unload();
		DhtDynamicPutter(Session^ sess);
		~DhtDynamicPutter(){}
		!DhtDynamicPutter();

	public:
		void RegisterPutCallback(array<byte, 1>^ key, PutterDelegate^ callback, array<byte, 1>^ salt);
		void UnregisterPutCallback(array<byte, 1>^ key, array<byte,1>^ salt);
		void UnregisterPutCallback(PutterDelegate^ callback);
		///<summary>
		///Sign mutable item, v: value, salt: salt, seq: currentseq, pk: pubkey of length 32, sk: privkey of length 64, sig: output buffer of size 64
		///</summary>
		static void SignMutableItem(array<byte, 1>^ sig, array<byte, 1>^ v, array<byte, 1>^ salt, System::UInt64 seq, array<byte, 1>^ pk, array<byte, 1>^ sk);
	};
}