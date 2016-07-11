
/*===================================================================
	File: StateManager.h
=====================================================================*/

#ifndef __STATEMANAGER_H__
#define __STATEMANAGER_H__

// forward declare
class IState;

class StateManager
{
	public:
		StateManager();
		~StateManager();

		void SetSecondaryManager( StateManager* pSecondaryManager );

		void Update( float deltaTime );
		void Render();
		int Resize(int windowWidth, int windowHeight);

		void ChangeState( IState* newState );
		void ChangeSecondaryState( IState* newState );

		IState* GetCurrentState() { return m_pCurrentState; }
		StateManager* GetSecondaryStateManager();

		void SetWaitingOnTransitionOut( bool state ) { m_WaitingOnTransitionOut = state; }
		void SetWaitingOnTransitionIn( bool state )	{ m_WaitingOnTransitionIn = state; }
	
	private:
		IState* m_pCurrentState;
		StateManager* m_pSecondaryManager;

		glm::ivec2 m_WindowDims;

		bool m_WaitingOnTransitionOut;
		bool m_WaitingOnTransitionIn;
		IState* m_pStoredTransitionState;
};

#endif // __STATEMANAGER_H__

