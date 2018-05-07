#pragma once

#include "mono_config.h"
#include "mono_type_traits.h"

#include "mono_object.h"
#include "mono_class_field.h"
#include "mono_class_property.h"

#include <cassert>
#include <string>

namespace mono
{

class mono_domain;
class mono_method;
class mono_class;
class mono_assembly;

class mono_class_instance : public mono_object
{
public:
	explicit mono_class_instance(MonoObject* obj);
	explicit mono_class_instance(mono_assembly* assembly, mono_domain* domain, MonoClass* cls);

	mono_class_instance(mono_class_instance&& o);
	auto operator=(mono_class_instance&& o) -> mono_class_instance&;

	auto get_method(const std::string& name, int argc = 0) -> mono_method;

	template <typename function_signature_t>
	auto get_method(const std::string& name);

	auto get_class() -> mono_class;

	template <typename T>
	auto get_field_value(const mono_class_field& field) const;

	template <typename T>
	void get_field_value(const mono_class_field& field, T& val) const;

	template <typename T>
	void set_field_value(const mono_class_field& field, const T& val) const;
    
private:
	MonoClass* class_ = nullptr;
	mono_assembly* assembly_ = nullptr;
};

template <typename function_signature_t>
auto mono_class_instance::get_method(const std::string& name)
{
	constexpr auto arg_count = function_traits<function_signature_t>::arity;
	auto func = get_method(name, arg_count);
	return func.template get_thunk<function_signature_t>();
}

template <typename T>
auto mono_class_instance::get_field_value(const mono_class_field& field) const
{
    T val{};
	get_field_value(field, val);
	return val;
}

template <typename T>
void mono_class_instance::get_field_value(const mono_class_field& field, T& val) const
{
	assert(object_);
	assert(field.get_internal_ptr());
	
	void* arg = nullptr;
	if(field.is_valuetype())
	{
        arg = reinterpret_cast<void*>(&val);
	}
	else
	{
        assert(false && "Unsupported non valuetype field getter");
	}
	
	mono_field_get_value(object_, field.get_internal_ptr(), arg);
}

template <typename T>
void mono_class_instance::set_field_value(const mono_class_field& field, const T& val) const
{
	assert(object_);
	assert(field.get_internal_ptr());
	
	void* arg = nullptr;
	if(field.is_valuetype())
	{
        arg = const_cast<void*>(reinterpret_cast<const void*>(&val));
	}
	else
	{
        assert(false && "Unsupported non valuetype field setter");
	}
	
	mono_field_set_value(object_, field.get_internal_ptr(), arg);
}

} // namespace mono
