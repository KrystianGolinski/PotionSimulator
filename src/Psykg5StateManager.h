#ifndef PSYKG5STATEMANAGER_H  
#define PSYKG5STATEMANAGER_H  

enum GameState
{
	MainMenu,
	Playing,
	Paused,
	GameOver,
	Win,
	Information
};

class Psykg5StateManager  
{  
public:  
	GameState GetCurrentState();  
	void SetCurrentState(GameState newState);  
};  

#endif
