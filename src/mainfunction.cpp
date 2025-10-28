#include "header.h"
#include <ctime>
#include "Psykg5GameEngine.h"
#include "Psykg5StateManager.h"


const int BaseScreenWidth = 800;
const int BaseScreenHeight = 900;

int doProgram(int argc, char *argv[])
{ 
	int GameState = 0;

	Psykg5StateManager MainStateManager;
	Psykg5GameEngine MainGameEngine(MainStateManager);

	char buf[1024];
	sprintf(buf, "Psykg5 Potion Simulator");

	GameState = MainGameEngine.initialise(buf, BaseScreenWidth, BaseScreenHeight, "Cornerstone Regular.ttf", 24);
	MainGameEngine.lockBackgroundForDrawing();
	MainGameEngine.virtSetupBackgroundBuffer(0);
	MainGameEngine.unlockBackgroundForDrawing();
	MainGameEngine.redrawDisplay();
	GameState = MainGameEngine.mainLoop();
	MainGameEngine.deinitialise();
	return GameState;
} 

int main(int argc, char *argv[])
{
	::srand( (unsigned int)time(0));

	int iResult = doProgram( argc, argv );

	ImageManager::destroyImageManager();

	return iResult;
}
