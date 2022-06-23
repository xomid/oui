#include "oui_variable.h"

_Variable::_Variable() : name("<unset>") {

}

_Variable::_Variable(std::string name) : name(name) {

}

std::string _Variable::get_name() const { 
	return name; 
}
