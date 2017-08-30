#pragma once
#include "libtorrent/lazy_entry.hpp"
#include "Utils.h"
#include "DataType.h"
namespace Ragnar
{
	namespace Unsafe
	{
		///<summary>
		///Only implement read part
		///</summary>
		public ref class LazyEntry
		{
			libtorrent::lazy_entry* ent;
		protected:
			
		public:
			static LazyEntry^ Wrap(libtorrent::lazy_entry* ent) {
				if (ent == NULL) return nullptr;

				auto e = gcnew LazyEntry();
				e->ent = ent;
				return e;
			}
			property DataType Type{DataType get() {
				auto ltype = ent->type();
				switch (ltype)
				{
				case libtorrent::lazy_entry::entry_type_t::none_t:
					return DataType::UNDEFINED;
				case libtorrent::lazy_entry::entry_type_t::dict_t:
					return DataType::DICTIONARY;
				case libtorrent::lazy_entry::entry_type_t::list_t:
					return DataType::LIST;
				case libtorrent::lazy_entry::entry_type_t::string_t:
					return DataType::STRING;
				case libtorrent::lazy_entry::entry_type_t::int_t:
					return DataType::INT;
				default:
					throw gcnew InvalidCastException("Unknown type");
				}
			}}

			char const* String_Ptr() { return ent->string_ptr(); }
			property int String_Length { int get()  { return ent->string_length(); }}
			System::String^ String_Val() {
				std::string val = ent->string_value();
				return Utils::GetManagedStringFromStandardString(val);
			}

			LazyEntry^ Dict_Find(char const* name) {
				libtorrent::lazy_entry* ret = ent->dict_find(name);
				return Wrap(ret);
			}
			LazyEntry^ Dict_Find(System::String^ name) {
				auto s = Utils::GetStdStringFromManagedString(name);
				return Dict_Find(s.c_str());
			}
			LazyEntry^ Dict_Find(char const* name, DataType type) {
				libtorrent::lazy_entry* ret = ent->dict_find(name);
				auto ret_ = Wrap(ret);
				if (ret_->Type != type) return nullptr;
				return ret_;
			}
			LazyEntry^ Dict_Find(System::String^ name, DataType type) {
				auto s = Utils::GetStdStringFromManagedString(name);
				return Dict_Find(s.c_str(), type);
			}

			System::Tuple<System::String^, LazyEntry^>^ Dict_At(int i) {
				auto ret = ent->dict_at(i);
				auto key = Utils::GetManagedStringFromStandardString(ret.first);
				return System::Tuple::Create(key, Wrap((libtorrent::lazy_entry*)ret.second));
			}

			boost::int64_t Dict_FindInt(System::String^ name) { return Dict_FindInt(name, 0); }
			boost::int64_t Dict_FindInt(System::String^ name, boost::int64_t default_val) {
				auto key = Utils::GetStdStringFromManagedString(name);
				return ent->dict_find_int_value(key.c_str(), default_val);
			}
			int Dict_Size() { return ent->dict_size(); }

			LazyEntry^ List_At(int i) {
				return Wrap(ent->list_at(i));
			}
			LazyEntry^ List_At(int i, DataType type) {
				auto ret = List_At(i);
				if (ret->Type != type) return nullptr;
				return ret;
			}
			boost::int64_t List_IntAt(int i) { return List_IntAt(i, 0); }
			boost::int64_t List_IntAt(int i, boost::int64_t default_val) { return ent->list_int_value_at(i, default_val); }
			int List_Size() { return ent->list_size(); }

			boost::int64_t Int_Value() { return ent->int_value(); }
		};
	}
}