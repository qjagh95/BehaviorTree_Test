#include "pch.h"
#include <Windows.h>
#include "BehaviorTree.h"
#include "TreeManager.h"

class ChairUp : public BehaviorTree::Action
{
public:
	ChairUp() {}
	~ChairUp() {}

	int Update(float DeltaTime) override
	{
		cout << "의자에서 일어난다" << endl;
		return BAT_SUCCED;
	}
	
	void Ending(float DeltaTime) override
	{
		cout << "ChairUp End함수 호출" << endl;
	}
};

class RandomSelectAction1 : public BehaviorTree::Action
{
public:
	RandomSelectAction1() { }
	~RandomSelectAction1() {}

	int Update(float DeltaTime) override
	{
		cout << "보리차를 산다." << endl;
		return BAT_SUCCED;
	}
};

class RandomSelectAction2 : public BehaviorTree::Action
{
public:
	RandomSelectAction2() { }
	~RandomSelectAction2() {}

	int Update(float DeltaTime) override
	{
		cout << "물을 산다." << endl;
		return BAT_SUCCED;
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
		return BAT_SUCCED;
	}
};

class SitDown : public BehaviorTree::Action
{
public:
	SitDown() { }
	~SitDown() {}

	int Update(float DeltaTime) override
	{
		cout << "의자에 앉는다." << endl;
		return BAT_RUNNING;
	}
};


int main() 
{
	srand(time(NULL));

	ChairUp* chairUp = new ChairUp();
	RandomSelectAction1* ranAction1 = new RandomSelectAction1();
	RandomSelectAction2* ranAction2 = new RandomSelectAction2();
	MyHome* myHome = new MyHome();
	SitDown* sitDown = new SitDown();

	BehaviorTree* myState = TreeManager::Get()->CreateBehaviorTree("PlayerState");
	myState->AddRootSequenceInAction("ChairUp", chairUp);

	myState->AddRootSequenceInSelector("RandomSelector");
	myState->AddSelectorInAction("RandomSelector", ranAction1);
	myState->AddSelectorInAction("RandomSelector", ranAction2);
	
	myState->AddRootSequenceInAction("MyHome", myHome);
	myState->AddRootSequenceInAction("sitDown", sitDown);

	myState->SetSelectorRandomProcess("RandomSelector", true);

	//게임루프 가정
	while (true)
	{	
		myState->Update(1.0f);
	}

}
