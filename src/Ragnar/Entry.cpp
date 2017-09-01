#include "Stdafx.h"
#include "Entry.h"
#include "Utils.h"
#include "DataType.h"

#include "libtorrent/entry.hpp"
using namespace System::Runtime::InteropServices;

namespace Ragnar
{
	namespace Unsafe
	{
		Entry^ Entry::Wrap(libtorrent::entry* entry)
		{
			if (entry->type() == libtorrent::entry::data_type::undefined_t)
				return gcnew Entry(entry);
			else if (entry->type() == libtorrent::entry::data_type::int_t)
				return gcnew EntryInteger(entry);
			else if (entry->type() == libtorrent::entry::data_type::string_t)
				return gcnew EntryString(entry);
			else if (entry->type() == libtorrent::entry::data_type::list_t)
				return gcnew EntryList(entry);
			else if (entry->type() == libtorrent::entry::data_type::dictionary_t)
				return gcnew EntryDictionary(entry);
			return nullptr;
		}
		Entry^ Entry::Wrap(libtorrent::entry const& entry)
		{
			auto ret = Wrap(new libtorrent::entry(entry));
			ret->orphan = true;
			return ret;
		}
		Entry^ Entry::Decode(array<byte, 1>^ data)
		{
			pin_ptr<byte> data_ = &data[0];
			libtorrent::entry*ent = new libtorrent::entry(libtorrent::bdecode((byte*)data_, (byte*)data_ + data->Length));
			auto wrap = Entry::Wrap(ent);
			wrap->orphan = true;
			return wrap;
		}
		array<byte, 1>^ Entry::Encode()
		{
			return Utils::GetByteArrayFromLibtorrentEntry(*entry);
		}
		DataType Entry::Type::get()
		{
			return DataType(entry->type());
		}
		Entry::~Entry() { if (orphan&&entry != NULL) { delete entry; entry = NULL; } }

		EntryInteger::EntryInteger(libtorrent::entry* entry) : Entry(entry)	{ int_ = &entry->integer(); }
		EntryString::EntryString(libtorrent::entry* entry) : Entry(entry) { str = &entry->string(); }
		EntryList::EntryList(libtorrent::entry* entry) : Entry(entry) { list = &entry->list(); }
		EntryDictionary::EntryDictionary(libtorrent::entry* entry) : Entry(entry) { dict = &entry->dict(); }

		EntryInteger::EntryInteger(__int64 value) : EntryInteger(new libtorrent::entry(value)) { orphan = true; }

		void EntryDictionary::Add(System::String^ key, Entry^ value)
		{
			if (key == nullptr) throw gcnew System::ArgumentNullException("key is null");
			if (value == nullptr) throw gcnew System::ArgumentNullException("value is null");
			std::string k_ = Utils::GetStdStringFromManagedString(key);
			if (dict->find(k_) != dict->end()) throw gcnew System::ArgumentException("An element with the same key already exists");
			dict->insert(std::make_pair(k_, *value->entry));
		}
		void EntryDictionary::Update(System::String^ key, Entry^ value)
		{
			if (key == nullptr) throw gcnew System::ArgumentNullException("key is null");
			if (value == nullptr) throw gcnew System::ArgumentNullException("value is null");
			std::string k_ = Utils::GetStdStringFromManagedString(key);
			(*dict)[k_] = *value->entry;
		}
		void EntryDictionary::Clear()
		{
			dict->clear();
		}
		bool EntryDictionary::ContainsKey(System::String^ key)
		{
			if (key == nullptr) throw gcnew System::ArgumentNullException("key is null");
			std::string k_ = Utils::GetStdStringFromManagedString(key);
			return dict->find(k_) != dict->end();
		}
		bool EntryDictionary::Remove(System::String^ key)
		{
			if (key == nullptr) throw gcnew System::ArgumentNullException("key is null");
			std::string k_ = Utils::GetStdStringFromManagedString(key);
			auto it = dict->find(k_);
			if (it == dict->end()) return false;
			dict->erase(it);
			return true;
		}
		Entry^ EntryDictionary::default::get(System::String^ key)
		{
			if (key == nullptr) throw gcnew System::ArgumentNullException("key is null");
			std::string k_ = Utils::GetStdStringFromManagedString(key);
			auto it = dict->find(k_);
			if (it == dict->end()) return nullptr;
			return Entry::Wrap(&it->second);
		}

		bool EntryDictionary::TryGetValue(System::String^ key, Entry^% value)
		{
			if (key == nullptr) throw gcnew System::ArgumentNullException("key is null");
			std::string k_ = Utils::GetStdStringFromManagedString(key);
			auto it = dict->find(k_);
			if (it == dict->end()) return false;
			value = Entry::Wrap(&it->second);
			return true;
		}

		System::Collections::Generic::ICollection<System::String^>^ EntryDictionary::Keys::get()
		{
			auto klist = gcnew System::Collections::Generic::List<System::String^>(dict->size());
			auto it = dict->begin(), ite = dict->end();
			for (; it != ite; ++it)
				klist->Add(Utils::GetManagedStringFromStandardString(it->first));
			return klist;
		}

		EntryString::EntryString(System::String^ str) : EntryString(new libtorrent::entry(Utils::GetStdStringFromManagedString(str))) {
			orphan = true;
		}
		EntryString::EntryString(char const*buf, int len) : EntryString(new libtorrent::entry(std::string(buf, len))) {
			orphan = true;
		}

		void EntryString::Append(char val)
		{
			str->push_back(val);
		}

		void EntryString::Append(array<char, 1>^ val, int idx, int len)
		{
			if (val == nullptr) throw gcnew System::ArgumentNullException("val cannot be null");
			if (idx<0 || idx >val->Length) throw gcnew System::ArgumentException("idx out of range");
			if (len<0 || idx + len>val->Length) throw gcnew System::ArgumentException("len out of range");
			if (len == 0) return;
			pin_ptr<char> val_ = &val[0];
			str->append((char*)val_ + idx, (char*)val_ + idx + len);
		}
		void EntryString::Append(array<byte, 1>^ val, int idx, int len)
		{
			if (val == nullptr) throw gcnew System::ArgumentNullException("val cannot be null");
			if (idx<0 || idx >val->Length) throw gcnew System::ArgumentException("idx out of range");
			if (len<0 || idx + len>val->Length) throw gcnew System::ArgumentException("len out of range");
			if (len == 0) return;
			pin_ptr<byte> val_ = &val[0];
			str->append((char*)val_ + idx, (char*)val_ + idx + len);
		}
		void EntryString::CopyTo(array<char, 1>^ arr, int idx)
		{
			if (arr == nullptr) throw gcnew System::ArgumentNullException("arr cannot be null");
			if (idx < 0) throw gcnew System::ArgumentOutOfRangeException("idx must not be negative");
			if (idx + str->size() > arr->Length) throw gcnew System::ArgumentException("buffer too small");
			pin_ptr<char> arr_ = &arr[0];
			byte*p = (byte*)arr_ + idx;
			for (int i = 0; i < str->size(); i++)
				p[i] = str->at(i);
		}
		void EntryString::CopyTo(array<byte, 1>^ arr, int idx)
		{
			if (arr == nullptr) throw gcnew System::ArgumentNullException("arr cannot be null");
			if (idx < 0) throw gcnew System::ArgumentOutOfRangeException("idx must not be negative");
			if (idx + str->size() > arr->Length) throw gcnew System::ArgumentException("buffer too small");
			pin_ptr<byte> arr_ = &arr[0];
			byte*p = (byte*)arr_ + idx;
			for (int i = 0; i < str->size(); i++)
				p[i] = str->at(i);
		}
		void EntryString::Insert(int p, char item)
		{
			if (p<0 || p>str->size()) throw gcnew System::ArgumentOutOfRangeException("pos out of range");
			str->insert(str->begin() + p, item);
		}
		void EntryString::Insert(int p, array<char, 1>^ arr, int idx, int len)
		{
			if (arr == nullptr) throw gcnew System::ArgumentNullException("arr cannot be null");
			if (idx<0 || idx >arr->Length) throw gcnew System::ArgumentException("idx out of range");
			if (len<0 || idx + len>arr->Length) throw gcnew System::ArgumentException("len out of range");
			if (p<0 || p>str->size()) throw gcnew System::ArgumentOutOfRangeException("pos out of range");
			pin_ptr<char> arr_ = &arr[0];
			str->insert(p, (char*)arr_ + idx, len);
		}
		void EntryString::Insert(int p, array<byte, 1>^ arr, int idx, int len)
		{
			if (arr == nullptr) throw gcnew System::ArgumentNullException("arr cannot be null");
			if (idx<0 || idx >arr->Length) throw gcnew System::ArgumentException("idx out of range");
			if (len<0 || idx + len>arr->Length) throw gcnew System::ArgumentException("len out of range");
			if (p<0 || p>str->size()) throw gcnew System::ArgumentOutOfRangeException("pos out of range");
			pin_ptr<byte> arr_ = &arr[0];
			str->insert(p, (char*)arr_ + idx, len);
		}

		void EntryString::RemoveAt(int p)
		{
			if (p<0 || p >= str->size()) throw gcnew System::ArgumentOutOfRangeException("pos out of range");
			str->erase(p);
		}
		void EntryString::RemoveRange(int p, int len)
		{
			if (p<0 || p > str->size()) throw gcnew System::ArgumentOutOfRangeException("pos out of range");
			if (len<0 || p + len>str->size()) throw gcnew System::ArgumentOutOfRangeException("len out of range");
			str->erase(p, len);
		}
		int EntryString::Count::get() { return str->size(); }
		char EntryString::default::get(int key)
		{
			if (key<0 || key >= str->size()) throw gcnew System::ArgumentOutOfRangeException("index out of range");
			return str->at(key);
		}
		void EntryString::default::set(int key, char val)
		{
			if (key<0 || key >= str->size()) throw gcnew System::ArgumentOutOfRangeException("index out of range");
			(*str)[key] = val;
		}
		array<byte, 1>^ EntryString::ToBytes()
		{
			return Utils::GetManagedBinaryFromStandardString(*str);
		}
		array<char, 1>^ EntryString::ToChars()
		{
			auto bytes = gcnew array<char>(str->size());
			if (str->size() == 0) return bytes;
			pin_ptr<char> bytes_ = &bytes[0];
			for (int i = 0; i < str->size(); i++)
				bytes_[i] = str->at(i);
			return bytes;
		}
		void EntryString::FromBytes(array<byte, 1>^ data, int idx, int len)
		{
			if (data == nullptr) throw gcnew System::ArgumentNullException("arr cannot be null");
			if (idx<0 || idx >data->Length) throw gcnew System::ArgumentException("idx out of range");
			if (len<0 || idx + len>data->Length) throw gcnew System::ArgumentException("len out of range");
			if (len == 0) {
				str->clear();
				return;
			}
			pin_ptr<byte> data_ = &data[0];
			str->clear();
			str->append((char*)data_ + idx, (char*)data_ + idx + len);
		}
		void EntryString::FromChars(array<char, 1>^ data, int idx, int len)
		{
			if (data == nullptr) throw gcnew System::ArgumentNullException("data cannot be null");
			if (idx<0 || idx >data->Length) throw gcnew System::ArgumentException("idx out of range");
			if (len<0 || idx + len>data->Length) throw gcnew System::ArgumentException("len out of range");
			pin_ptr<char> data_ = &data[0];
			str->clear();
			str->append((char*)data_ + idx, (char*)data_ + idx + len);
		}

		void EntryList::Append(Entry^ val)
		{
			if (val == nullptr) throw gcnew System::ArgumentNullException("val cannot be null");
			list->push_back(*val->entry);
		}
		void EntryList::Clear()	{ list->clear(); }
		int EntryList::Count::get() { return list->size(); }
		void EntryList::Insert(EntryList::EntryListIterator^ it, Entry^ item)
		{
			if (item == nullptr) throw gcnew System::ArgumentNullException("item is null");
			if (it == nullptr) throw gcnew System::ArgumentNullException("iterator is null");
			list->insert(*(it->p), *item->entry);
		}
		void EntryList::Delete(EntryList::EntryListIterator^ it)
		{
			if (it == nullptr) throw gcnew System::ArgumentNullException("iterator is null");
			list->erase(*(it->p));
		}
		EntryList::EntryListIterator^ EntryList::Begin() { return gcnew EntryList::EntryListIterator(list->begin(), list); }
		EntryList::EntryListIterator^ EntryList::End() { return gcnew EntryList::EntryListIterator(list->end(), list); }


		EntryList::EntryListIterator::EntryListIterator(libtorrent::list_it &out, libtorrent::list_type* list)
		{
			this->p = new libtorrent::list_it(out);
			this->list = list;
		}
		EntryList::EntryListIterator::EntryListIterator(EntryListIterator^ obj) : EntryListIterator(*obj->p, obj->list) { }
		bool EntryList::EntryListIterator::Next()
		{
			if (list->end() == *this->p) return false;
			++(*this->p);
			return true;
		}
		bool EntryList::EntryListIterator::Prev()
		{
			if (list->begin() == *this->p) return false;
			--(*this->p);
			return true;
		}
		bool EntryList::EntryListIterator::IsEnd::get() { return (list->end() == *this->p); }
		Entry^ EntryList::EntryListIterator::Item::get()
		{
			if (IsEnd) return nullptr;
			return Entry::Wrap(&**p);
		}
		void EntryList::EntryListIterator::Item::set(Entry^ value)
		{
			if (IsEnd) throw gcnew InvalidOperationException("Cannot set value of end-ptr");
			**p = *value->entry;
		}
	}
}