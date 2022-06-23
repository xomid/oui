#pragma once
#include "oui_basics.h"

struct OUI_API _Variable {
	std::string name;
	_Variable();
	_Variable(std::string name);
	virtual std::string get_value() const = 0;
	std::string get_name() const;
};

template<typename T>
struct Variable : public _Variable {
	T* value;

	Variable(std::string name, T* value) : _Variable(name), value(value) {
	}

	std::string get_value() const override {
		return value ? std::to_string(*value) : "na";
	}
};


