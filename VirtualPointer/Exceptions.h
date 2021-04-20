#pragma once
#include <exception>

struct NullPointerException final : public std::exception {
	const char * what() const override
	{
		return "Attempt to access a null pointer";
	}
};
