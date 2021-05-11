#pragma once

#include "Elements.hpp"
#include <vector>
#include <unordered_map>

enum class ElementType
{
	Line, Text, Circle
};

enum class ChangeType
{
	Create, Edit, Delete
};

struct Change
{
	ElementType elementType;
	uint64_t elementId;
	void *previousValue = nullptr;

	template<typename T>
	void setPreviousValue(const T& val)
	{
		previousValue = malloc(sizeof(T));
		new (previousValue) T();
		T* k = reinterpret_cast<T*>(previousValue);
		*k = val;
		elementId = val.id;
	}
};

struct ElementChange
{
	uint64_t elementId;
	uint64_t timelineIndex;
};

// if user moves multiple elements at the same time we want
// to undo them all simultaniously
struct TimeFrame
{
	ChangeType changeType;
	std::vector<ElementChange> changes;
};

struct History
{
	std::unordered_map<uint64_t, std::vector<Change>> elementsChanges;
	std::vector<TimeFrame> timeFrames;
	int currentHistoryIndex = 0;

	std::vector<Change> collectChangesForTimeFrameIndex(int index)
	{
		std::vector<Change> result;
		auto &tf = timeFrames[index];

		for (auto& ec : tf.changes)
		{
			result.push_back(elementsChanges[ec.elementId][ec.timelineIndex]);
		}
		
		return result;
	}

	std::vector<Change> collectPreviousStateForTimeFrameIndex(int index)
	{
		std::vector<Change> result;
		auto &tf = timeFrames[index];

		for (auto& ec : tf.changes)
		{
			if (ec.timelineIndex > 0)
			{
				result.push_back(elementsChanges[ec.elementId][ec.timelineIndex-1]);
			}
			else
			{
				std::cout << "(3894)Error: element does not have previous state\n";
			}
		}
		
		return result;
	}

	void addChanges(const std::vector<Change>& changes, ChangeType t)
	{
		if (timeFrames.size() > 0 && currentHistoryIndex != timeFrames.size()-1)
		{
			// free previous values
			for (int i = currentHistoryIndex+1; i < timeFrames.size(); ++i)
			{
				for (auto& c : timeFrames[i].changes)
				{
					free(elementsChanges[c.elementId][c.timelineIndex].previousValue);
				}
			}
			for (auto& c : timeFrames[currentHistoryIndex+1].changes)
			{
				elementsChanges[c.elementId].erase(elementsChanges[c.elementId].begin()
						+ c.timelineIndex, elementsChanges[c.elementId].end());
			}
			timeFrames.erase(timeFrames.begin()+currentHistoryIndex+1, timeFrames.end());
		}

		TimeFrame tf;
		tf.changeType = t;

		for (auto& change : changes)
		{
			ElementChange ec;
			ec.elementId = change.elementId;

			if (elementsChanges.find(ec.elementId) != elementsChanges.end())
			{
				elementsChanges[ec.elementId].push_back(change);
			}
			else
			{
				elementsChanges[ec.elementId] = { change };
			}
			ec.timelineIndex = elementsChanges[ec.elementId].size()-1;

			tf.changes.push_back(ec);
		}

		timeFrames.push_back(tf);
		
		currentHistoryIndex = timeFrames.size()-1;
	}
};

