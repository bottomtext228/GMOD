#pragma once

enum lifeStates {
	alive = 0,
	dead = 2,
};

namespace offsets {
	constexpr uintptr_t entityList = 0x6ADF2C;
	constexpr uintptr_t viewMatrix = 0x3FFD5C;
	constexpr uintptr_t viewMatrixOffset = 0x2D4;
	constexpr uintptr_t localPlayer = 0x691D94;
}

namespace vars {
	bool init = false;
	bool wallHackState = false;
	bool linesState = false;
	bool ESPState = false;
	bool renderHealthAndNick = false;
	//bool renderEntity = false;
	//bool debug;
	bool menu = false;
	float boxESPSize = 15.0;
	float* matrix;
	int resX;
	int resY;
	DWORD client;
	DWORD engine;
};
