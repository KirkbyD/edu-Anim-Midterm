#pragma once
namespace nAI {
	// IDLE = 1
	// SEEK = 2
	// APPROACH = 4
	// PURSURE = 8
	// FLEE = 16
	// EVADE = 32
	// WANDER = 64
	// FOLLOW_PATH = 128
	enum class eAIBehviours {
		IDLE = 1,
		SEEK = 2,
		APPROACH = 4,
		PURSURE = 8,
		FLEE = 16,
		EVADE = 32,
		WANDER = 64,
		FOLLOW_PATH = 128
	};

	enum class ePlayerTypes {
		PLAYER,
		FRIENDLY,
		NEUTRAL,
		HOSTILE
		// D&D / PATHFINDER HAS MORE STATES POSSIBLY EXPAND LATER...
	};

	enum class eFormationBehaviours {
		NONE,
		CIRCLE,
		SQUARE,
		WEDGE,
		LINE,
		COLUMN
	};
}
