#include "pch.h"
#include <Windows.h>
#include "BehaviorTree.h"
#include "BTManager.h"

class ChairUp : public BehaviorTree::Action
{
public:
	ChairUp() {}
	~ChairUp() {}

	int Update(float DeltaTime) override
	{
		cout << "의자에서 일어난다" << endl;
		return ACTION_RUNNING;
	}
	
	void Ending(float DeltaTime) override
	{
		cout << "ChairUp End함수 호출" << endl;
	}
};

class Action1 : public BehaviorTree::Action
{
public:
	Action1() {}
	~Action1() {}

	void Init() override
	{
		cout << "Action1 Start" << endl;
	}

	int Update(float DeltaTime) override
	{
		cout << "보리차를 산다." << endl;

		if(GetAsyncKeyState(VK_SPACE) & 0x8000)
			return ACTION_RUNNING;
		else
			return ACTION_FALSE;
	}
};

class Action2 : public BehaviorTree::Action
{
public:
	Action2() { }
	~Action2() {}

	int Update(float DeltaTime) override
	{
		cout << "물을 산다." << endl;
		return ACTION_FALSE;
	}
};

class MyHome : public BehaviorTree::Action
{
public:
	MyHome() {}
	~MyHome() {}

	int Update(float DeltaTime) override
	{
		cout << "내 자리로 간다." << endl;
		return ACTION_SUCCED;
	}
};

class SitDown : public BehaviorTree::Action
{
public:
	SitDown() {}
	~SitDown() {}

	int Update(float DeltaTime) override
	{
		cout << "의자에 앉는다" << endl;
		return ACTION_SUCCED;
	}
};

int main() 
{
	srand(time(NULL));

	//ChairUp* chairUp = new ChairUp();
	Action1* ranAction1 = new Action1();
	Action2* ranAction2 = new Action2();
	MyHome* myHome = new MyHome();
	SitDown* sitDown = new SitDown();

	BehaviorTree* myState = BTManager::Get()->CreateBehaviorTree("PlayerState", BT_SEQUENCE);
	myState->AddRootSequenceInSelector("RandomSelector");
	myState->AddSelectorInAction("RandomSelector", "Action1", ranAction1);
	myState->AddSelectorInAction("RandomSelector", "Action2", ranAction2);
	
	myState->AddRootSequenceInAction("MyHome", "Action3", myHome);
	myState->AddRootSequenceInAction("sitDown", "Action3", sitDown);

	//게임루프 가정
	while (true)
	{	
  		myState->Update(1.0f);
	}

	delete myState;
	BTManager::Delete();
}
