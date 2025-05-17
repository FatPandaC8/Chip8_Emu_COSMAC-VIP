#include "chip8def.h"
#include "window.h"

int main(int argc, char** argv)
{
	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]); //1 for 1000Hz
	char const* romFilename = argv[3];

	Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

	Chip8 SuperChip8;
	SuperChip8.LoadROM(romFilename);

	int pitch = sizeof(SuperChip8.video[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform.input(SuperChip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			SuperChip8.Cycle();

			platform.update(SuperChip8.video, pitch);
		}
	}

	return 0;
}
