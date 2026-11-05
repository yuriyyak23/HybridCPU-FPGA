#pragma once

// ----------------------------------------------------------------------------------------
// Copyright © 2004 - 2025 Tangible Software Solutions Inc.
// This class can be used by anyone provided that the copyright notice remains intact.
//
// This class is used to convert C# events to C++.
// ----------------------------------------------------------------------------------------
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

// Base for event payloads converted from System.EventArgs.
class EventArgs : public std::enable_shared_from_this<EventArgs> { public: virtual ~EventArgs() = default; };

template<typename T>
class TangibleEvent final
{
private:
	std::unordered_map<std::wstring, T> namedListeners;
	std::vector<T> anonymousListeners;

public:
	void addListener(const std::wstring &methodName, T namedEventHandlerMethod)
	{
		if (namedListeners.find(methodName) == namedListeners.end())
			namedListeners[methodName] = namedEventHandlerMethod;
	}

	void addListener(T unnamedEventHandlerMethod)
	{
		anonymousListeners.push_back(unnamedEventHandlerMethod);
	}

	void removeListener(const std::wstring &methodName)
	{
		if (namedListeners.find(methodName) != namedListeners.end())
			namedListeners.erase(methodName);
	}

	std::vector<T> listeners()
	{
		std::vector<T> allListeners;
		for (auto listener : namedListeners)
		{
			allListeners.push_back(listener.second);
		}
		allListeners.insert(allListeners.end(), anonymousListeners.begin(), anonymousListeners.end());
		return allListeners;
	}
};
