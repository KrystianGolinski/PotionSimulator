#include "Psykg5StateManager.h"

GameState CurrentState = GameState::MainMenu;

GameState Psykg5StateManager::GetCurrentState()
{
	return CurrentState;
}

void Psykg5StateManager::SetCurrentState(GameState newState)
{
	CurrentState = newState;
}
