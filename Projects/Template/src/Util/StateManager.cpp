
/*===================================================================
	File: StateManager.cpp
=====================================================================*/

#include "IState.h"
#include "StateManager.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
StateManager::StateManager()
{
	m_pCurrentState = 0;
	m_pSecondaryManager = 0;

	m_WaitingOnTransitionOut = false;
	m_WaitingOnTransitionIn = false;
	m_pStoredTransitionState = nullptr;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
StateManager::~StateManager()
{
	if( m_pCurrentState )
	{
		// bail out of old state
		m_pCurrentState->Exit();

		// free memory
		delete m_pCurrentState;
		m_pCurrentState = nullptr;
	}

	m_pSecondaryManager = nullptr;
}

/////////////////////////////////////////////////////
/// Method: SetSecondaryManager
/// Params: [in]pSecondaryManager
///
/////////////////////////////////////////////////////
void StateManager::SetSecondaryManager( StateManager* pSecondaryManager )
{
	m_pSecondaryManager = pSecondaryManager;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void StateManager::Update( float deltaTime )
{
	if( m_WaitingOnTransitionOut )
	{
		// do nothing
	}
	else if( m_WaitingOnTransitionIn )
	{
		// do nothing
	}
	else
	{
		if( m_pCurrentState )
			m_pCurrentState->Update( deltaTime );
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void StateManager::Render()
{
	if( m_WaitingOnTransitionOut )
	{
		// finished ?
		if( !m_pCurrentState->TransitionOut() )
		{
			m_pCurrentState->Exit();

			// free memory
			delete m_pCurrentState;
			m_pCurrentState = nullptr;

			// move to new state
			if( m_pStoredTransitionState )
			{
				m_pCurrentState = m_pStoredTransitionState;
				m_pCurrentState->Enter();

				m_pCurrentState->PrepareTransitionIn();
				if( m_pCurrentState->TransitionIn() )
					m_WaitingOnTransitionIn = true;
				
				// clear flags
				m_WaitingOnTransitionOut = false;
				m_pStoredTransitionState = nullptr;
			}
		}
	}
	else if( m_WaitingOnTransitionIn )
	{
		if( !m_pCurrentState->TransitionIn() )
		{
			// clear flags
			m_WaitingOnTransitionIn = false;
		}
	}
	else
	{
		if( m_pCurrentState )
			m_pCurrentState->Render();
	}
}

/////////////////////////////////////////////////////
/// Method: Resize
/// Params: [in]newState
///
/////////////////////////////////////////////////////
int StateManager::Resize(int windowWidth, int windowHeight)
{
	if (windowWidth <= 0)
		windowWidth = 1;

	if (windowHeight <= 0)
		windowHeight = 1;

	m_WindowDims.x = windowWidth;
	m_WindowDims.y = windowHeight;

	if (m_pCurrentState)
		m_pCurrentState->Resize(windowWidth, windowHeight);

	return 0;
}

/////////////////////////////////////////////////////
/// Method: ChangeState
/// Params: [in]newState
///
/////////////////////////////////////////////////////
void StateManager::ChangeState( IState* newState )
{
	// no new state, do a fast complete shutdown
	if( newState == 0 )
	{
		if( m_pCurrentState )
		{
			m_pCurrentState->Exit();
		
			// free memory
			delete m_pCurrentState;
			m_pCurrentState = nullptr;
		}
		return;
	}
	
	// old state
	if( m_pCurrentState )
	{
		// transition out of old state
		m_pCurrentState->PrepareTransitionOut();

		if( m_pCurrentState->TransitionOut() )
		{
			m_WaitingOnTransitionOut = true;
			m_pStoredTransitionState = newState;
			return;
		}
		else
		{
			// just remove this state and move to the next
			m_WaitingOnTransitionOut = false;
			m_pStoredTransitionState = nullptr;

			m_pCurrentState->Exit();

			// free memory
			delete m_pCurrentState;
			m_pCurrentState = nullptr;
		}
	}

	// new state
	if( newState )
	{
		m_pCurrentState = newState;
		m_pCurrentState->Enter();

		m_pCurrentState->PrepareTransitionIn();
		if( m_pCurrentState->TransitionIn() )
			m_WaitingOnTransitionIn = true;
	}
}

/////////////////////////////////////////////////////
/// Method: ChangeState
/// Params: [in]newState
///
/////////////////////////////////////////////////////
void StateManager::ChangeSecondaryState( IState* newState )
{
	if( m_pSecondaryManager )
		m_pSecondaryManager->ChangeState( newState );
}

/////////////////////////////////////////////////////
/// Method: GetSecondaryStateManager
/// Params: None
///
/////////////////////////////////////////////////////
StateManager* StateManager::GetSecondaryStateManager()
{
	return( m_pSecondaryManager );
}
