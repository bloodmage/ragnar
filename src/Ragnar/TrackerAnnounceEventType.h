#pragma once

namespace Ragnar
{
	[System::FlagsAttribute]
	public enum class TrackerAnnounceEvent : int
	{
		None = 0,
		Completed = 1,
		Starteed = 2,
		Stopped = 3
	};
}