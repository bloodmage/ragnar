#pragma once
#include <map>
#include <list>
#include <string>

namespace libtorrent
{
	class entry;
	typedef std::map<std::string, entry> dictionary_type;
	typedef std::string string_type;
	typedef std::list<entry> list_type;
	typedef std::list<entry>::iterator list_it;
	typedef __int64 integer_type;

}
namespace Ragnar
{
	namespace Unsafe
	{
		enum class DataType : int;
		ref class Entry;
		ref class EntryDictionary;
		ref class EntryString;
		ref class EntryList;
		ref class EntryInteger;
		ref class EntryListIterator;

		public ref class Entry
		{
		internal:
			libtorrent::entry * entry;
			bool orphan = false;
		protected:
			Entry(libtorrent::entry * entry) { this->entry = entry; }
		public:
			static Entry^ Wrap(libtorrent::entry* entry);
			static Entry^ Wrap(libtorrent::entry const& entry);
			static Entry^ Decode(array<byte, 1>^ data);
			property DataType Type { DataType get(); }
			~Entry();
			!Entry() { Entry::~Entry(); }
			array<byte, 1>^ Encode();
		};

		public ref class EntryDictionary : Entry
		{
		private:
			libtorrent::dictionary_type* dict;
		public:
			EntryDictionary(libtorrent::entry* entry);

			void Add(System::String^ key, Entry^ value);
			void Update(System::String^ key, Entry^ value);
			void Clear();
			bool ContainsKey(System::String^ key);
			bool Remove(System::String^ key);
			property Entry^ default[System::String^] {Entry^ get(System::String^ key); }

			///<summary>
			///Warning: handle becomes invalid and must be thrown when father changed
			///</summary>
			bool TryGetValue(System::String^ key, [System::Runtime::InteropServices::Out] Entry^% value);
			property System::Collections::Generic::ICollection<System::String^>^ Keys{ System::Collections::Generic::ICollection<System::String^>^ get(); }
		};

		///<summary>
		///A binary string representation
		///</summary>
		public ref class EntryString : Entry
		{
		private:
			libtorrent::string_type* str;
		public:
			EntryString(libtorrent::entry* entry);
			EntryString(System::String^ str);
			EntryString(char const*buf, int len);

			void Append(char val);
			void Append(array<char, 1>^ val, int idx, int len);
			void Append(array<byte, 1>^ val, int idx, int len);
			void CopyTo(array<char, 1>^ arr, int idx);
			void CopyTo(array<byte, 1>^ arr, int idx);
			void Insert(int p, char item);
			void Insert(int p, array<char, 1>^ arr, int idx, int len);
			void Insert(int p, array<byte, 1>^ arr, int idx, int len);
			void RemoveAt(int p);
			void RemoveRange(int p, int len);
			property int Count{int get(); }
			property char default[int] {char get(int key); void set(int key, char val); }
			array<byte, 1>^ ToBytes();
			array<char, 1>^ ToChars();
			void FromBytes(array<byte, 1>^ data, int idx, int len);
			void FromChars(array<char, 1>^ data, int idx, int len);
		};

		///<summary>
		///Double linked list
		///</summary>
		public ref class EntryList : Entry
		{
		private:
			libtorrent::list_type* list;
		public:
			ref class EntryListIterator
			{
			internal:
				libtorrent::list_it* p;
				libtorrent::list_type* list;
			public:
				EntryListIterator(libtorrent::list_it &out, libtorrent::list_type* list);
				EntryListIterator(EntryListIterator^ obj);
				~EntryListIterator() { if (p != NULL) { delete p; p = NULL; } }
				!EntryListIterator() { this->~EntryListIterator(); }
				bool Next();
				bool Prev();
				property bool IsEnd{bool get(); }
				property Entry^ Item{ Entry^ get(); void set(Entry^ value); }
			};

			EntryList(libtorrent::entry* entry);

			void Append(Entry^ val);
			///<summary>
			///All referencing iterators will be invalid and should be discarded
			///</summary>
			void Clear();
			property int Count{int get(); }
			EntryListIterator^ Begin();
			EntryListIterator^ End();
			///<summary>
			///Iterators referencing this position will be invalid
			///</summary>
			void Delete(EntryListIterator^ it);
			///<summary>
			///Element will be insert before iterator
			///</summary>
			void Insert(EntryListIterator^ it, Entry^ value);

		};

		public ref class EntryInteger : Entry
		{
		private:
			libtorrent::integer_type* int_;
		public:
			EntryInteger(libtorrent::entry* entry);
			EntryInteger(__int64 value);
			EntryInteger(unsigned __int64 value) : EntryInteger((__int64)value) {}

			property __int64 Value{__int64 get() { return *int_; } void set(__int64 value) { *int_ = value; } }
		};

	}
}