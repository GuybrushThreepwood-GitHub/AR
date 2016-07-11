
/*===================================================================
	File: IState.h
=====================================================================*/

#ifndef __ISTATE_H__
#define __ISTATE_H__

#include "Boot/Includes.h"

#ifndef __STATEMANAGER_H__
	#include "StateManager.h"
#endif // __STATEMANAGER_H__

class IState
{
	public:
		IState( StateManager& stateManager, int stateId=0 ) 
		{
			m_pStateManager = &stateManager;

			m_Id = stateId;
		}
		virtual ~IState() 
		{
			m_pStateManager = nullptr;
		}

		virtual void Enter() = 0;
		virtual void Exit() = 0;
		virtual int Resize(int windowWidth, int windowHeight) = 0;

		virtual void PrepareTransitionIn(){};
		virtual int TransitionIn() = 0;
		virtual void PrepareTransitionOut(){};
		virtual int TransitionOut() = 0;
		
		virtual void Update( float deltaTime ) = 0;
		virtual void Render() = 0;

		void ChangeState( IState* newState )
		{
			if( m_pStateManager )
				m_pStateManager->ChangeState( newState );
		}
		StateManager* GetStateManager()
		{
			return( m_pStateManager );
		}

		int GetId()							{ return m_Id; }

	protected:
		float m_LastDelta;
		StateManager* m_pStateManager;
		glm::ivec2 m_WindowDims;
		int m_Id;
};

#endif // __ISTATE_H__