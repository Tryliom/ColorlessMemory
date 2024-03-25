#pragma once

#include <string>

struct ClientId
{
	int Index = -1;

	[[nodiscard]] bool IsEmpty() const
	{
		return Index == -1;
	}

	bool operator==(const ClientId& other) const
	{
		return Index == other.Index;
	}
};

static constexpr ClientId EMPTY_CLIENT_ID { -1 };