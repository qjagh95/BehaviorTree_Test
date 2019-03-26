#include "pch.h"
#include "BehaviorTree.h"
#include "BTManager.h"

BehaviorTree::BehaviorTree()
{
	m_RootNode = NULL;
	m_RootSequence = NULL;
	m_RootSelector = NULL;
	m_CurAction = NULL;
	m_isOnePath = false;
	m_Type = ACTION_NONE;
	m_RootName = "RootNode";
	m_RootSequenceName = "RootSequence";
	m_RootSelectorName = "RootSelector";

	m_Count = 2;
}

BehaviorTree::~BehaviorTree()
{
	BTManager::Get()->Delete(m_TagName);

	if (m_RootSequence != NULL)
	{
		delete m_RootSequence;
		m_RootSequence = NULL;
	}

	if (m_RootSelector != NULL)
	{
		delete m_RootSelector;
		m_RootSelector = NULL;
	}

	if (m_RootNode != NULL)
	{
		delete m_RootNode;
		m_RootNode = NULL;
	}

	unordered_map<string, Selector*>::iterator StartIter1 = m_SelectorMap.begin();
	unordered_map<string, Selector*>::iterator EndIter1 = m_SelectorMap.end();

	for (; StartIter1 != EndIter1; StartIter1++)
		delete StartIter1->second;

	unordered_map<string, Sequence*>::iterator StartIter = m_SequenceMap.begin();
	unordered_map<string, Sequence*>::iterator EndIter = m_SequenceMap.end();

	for (; StartIter != EndIter; StartIter++)
		delete StartIter->second;


	unordered_map<string, Action*>::iterator StartIter2 = m_ActionMap.begin();
	unordered_map<string, Action*>::iterator EndIter2 = m_ActionMap.end();

	for (; StartIter2 != EndIter2; StartIter2++)
		delete StartIter2->second;
}

void BehaviorTree::Update(float DeltaTime)
{
	switch (m_Type)
	{
	case ACTION_RUNNING:
		m_Type = (BT_ACTION_TYPE)m_CurAction->Update(DeltaTime);
		break;
	case ACTION_NONE:
	case ACTION_SUCCED:
	case ACTION_FALSE:
		m_Type = (BT_ACTION_TYPE)m_RootNode->Update(DeltaTime);
		break;
	}
}

void BehaviorTree::AddRootSequenceInSequence(const string& NewSequenceKeyName)
{
	if (m_RootSequence == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	Sequence* newSequence = FindSequence(NewSequenceKeyName);

	if (newSequence == NULL)
	{
		newSequence = new Sequence();
		newSequence->SetTag(NewSequenceKeyName);
		newSequence->SetTreeName(m_TagName);
		newSequence->SetKeepAction(m_RootSequence);

		m_RootSequence->AddChild(newSequence);
		m_SequenceMap.insert(make_pair(NewSequenceKeyName, newSequence));
		m_Count++;
	}
}

void BehaviorTree::AddRootSequenceInSelector(const string & NewSelectorKeyName)
{
	if (m_RootSequence == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	Selector* newSelector = FindSelector(NewSelectorKeyName);

	if (newSelector != NULL)
		return;

	newSelector = new Selector();
	newSelector->SetTag(NewSelectorKeyName);
	newSelector->SetTreeName(m_TagName);
	newSelector->SetKeepAction(m_RootSequence);

	m_RootSequence->AddChild(newSelector);
	m_SelectorMap.insert(make_pair(NewSelectorKeyName, newSelector));
	m_Count++;

}

void BehaviorTree::AddRootSequenceInAction(const string& SequenceKeyName, const string& ActionName, Action* NewAction)
{
	if (m_RootSequence == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	NewAction->Init();
	NewAction->SetTag(SequenceKeyName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetKeepAction(m_RootSequence);

	m_RootSequence->AddChild(NewAction);
	m_ActionMap.insert(make_pair(ActionName, NewAction));

	m_Count++;
}

void BehaviorTree::AddRootSelectorInSequence(const string & NewSequenceKeyName)
{
	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootSelector == NULL)
	{
		assert(false);
		return;
	}

	Sequence* newSequence = new Sequence();
	newSequence->Init();
	newSequence->SetTag(NewSequenceKeyName);
	newSequence->SetTreeName(m_TagName);
	newSequence->SetKeepAction(m_RootSelector);

	m_RootSelector->AddChild(newSequence);
	m_SequenceMap.insert(make_pair(NewSequenceKeyName, newSequence));
	m_Count++;
}

void BehaviorTree::AddRootSelectorInSelector(const string & NewSelectorKeyName)
{
	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootSelector == NULL)
	{
		assert(false);
		return;
	}

	Selector* newSelector = new Selector();
	newSelector->Init();
	newSelector->SetTag(NewSelectorKeyName);
	newSelector->SetTreeName(m_TagName);
	newSelector->SetKeepAction(m_RootSelector);

	m_RootSelector->AddChild(newSelector);
	m_SelectorMap.insert(make_pair(NewSelectorKeyName, newSelector));
	m_Count++;
}

void BehaviorTree::AddRootSelectorInAction(const string & SelectorKeyName, const string& ActionName, Action * NewAction)
{
	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootSelector == NULL)
	{
		assert(false);
		return;
	}

	NewAction->Init();
	NewAction->SetTag(ActionName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetKeepAction(m_RootSelector);

	m_RootSelector->AddChild(NewAction);
	m_ActionMap.insert(make_pair(ActionName, NewAction));
	m_Count++;
}

void BehaviorTree::AddSequenceInAction(const string & SequenceKeyName, const string& ActionName, Action* NewAction)
{
	Sequence* getSequence = FindSequence(SequenceKeyName);

	if (getSequence == NULL)
	{
		assert(false);
		return;
	}

	NewAction->Init();
	NewAction->SetTag(ActionName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetKeepAction(getSequence);

	getSequence->AddChild(NewAction);
	m_ActionMap.insert(make_pair(ActionName, NewAction));
	m_Count++;
}

void BehaviorTree::AddSelectorInAction(const string & SelectorKeyName, const string& ActionName, Action * NewAction)
{
	Selector* getSelector = FindSelector(SelectorKeyName);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	NewAction->Init();
	NewAction->SetTag(ActionName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetKeepAction(getSelector);

	getSelector->AddChild(NewAction);
	m_ActionMap.insert(make_pair(ActionName, NewAction));
	m_Count++;
}

void BehaviorTree::AddSequenceInSelector(const string & SequenceKeyName, const string & SelectorKeyName)
{
	Sequence* getSequence = FindSequence(SequenceKeyName);

	if (getSequence == NULL)
	{
		assert(false);
		return;
	}

	Selector* newSelector = FindSelector(SelectorKeyName);

	if (newSelector != NULL)
	{
		assert(false);
		return;
	}

	newSelector = new Selector();
	newSelector->Init();
	newSelector->SetTag(SelectorKeyName);
	newSelector->SetTreeName(m_TagName);
	newSelector->SetKeepAction(getSequence);

	getSequence->AddChild(newSelector);
	m_SelectorMap.insert(make_pair(SelectorKeyName, newSelector));
	m_Count++;
}

void BehaviorTree::AddSelectorInSequence(const string & SelectorKeyName, const string & SequenceKeyName)
{
	Selector* getSelector = FindSelector(SelectorKeyName);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	Sequence* newSequence = FindSequence(SequenceKeyName);

	if (newSequence != NULL)
	{
		assert(false);
		return;
	}

	newSequence = new Sequence();
	newSequence->Init();
	newSequence->SetTag(SequenceKeyName);
	newSequence->SetTreeName(m_TagName);
	newSequence->SetKeepAction(getSelector);

	getSelector->AddChild(newSequence);
	m_SequenceMap.insert(make_pair(SequenceKeyName, newSequence));
	m_Count++;
}

void BehaviorTree::AddSequenceInSequence(const string & OldSequenceKey, const string & NewSequenceKey)
{
	Sequence* getSelector = FindSequence(OldSequenceKey);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	Sequence* newSequence = FindSequence(NewSequenceKey);

	if (newSequence != NULL)
	{
		assert(false);
		return;
	}

	newSequence = new Sequence();
	newSequence->Init();
	newSequence->SetTag(NewSequenceKey);
	newSequence->SetTreeName(m_TagName);
	newSequence->SetKeepAction(getSelector);

	getSelector->AddChild(newSequence);
	m_SequenceMap.insert(make_pair(NewSequenceKey, newSequence));
	m_Count++;
}

void BehaviorTree::SetSelectorRandomProcess(const string & SelectorKeyName, bool Value)
{
	Selector* getSelector = FindSelector(SelectorKeyName);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	getSelector->SetRandomProcess(Value);
}

void BehaviorTree::SequenceTickFuncDisable(const string & SequenceKeyName)
{
	Sequence* getSequence = FindSequence(SequenceKeyName);

	if (getSequence == NULL)
	{
		assert(false);
		return;
	}

	getSequence->DisableTickFunc();
}

void BehaviorTree::SequenceTickFuncEnable(const string & SequenceKeyName)
{
	Sequence* getSequence = FindSequence(SequenceKeyName);

	if (getSequence == NULL)
	{
		assert(false);
		return;
	}

	getSequence->EnableTickFunc();
}

void BehaviorTree::SelectorTickFuncDisable(const string & SelectorKeyName)
{
	Selector* getSelector = FindSelector(SelectorKeyName);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	getSelector->DisableTickFunc();
}

void BehaviorTree::SelectorTickFuncEnable(const string & SelectorKeyName)
{
	Selector* getSelector = FindSelector(SelectorKeyName);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	getSelector->EnableTickFunc();
}

void BehaviorTree::AddSelectorInSelector(const string & OldSelectorKey, const string & NewSelector)
{
	Selector* getSelector = FindSelector(OldSelectorKey);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	Selector* newSelector = FindSelector(NewSelector);

	if (newSelector != NULL)
	{
		assert(false);
		return;
	}

	newSelector = new Selector();
	newSelector->Init();
	newSelector->SetTag(NewSelector);
	newSelector->SetTreeName(m_TagName);
	newSelector->SetKeepAction(getSelector);

	getSelector->AddChild(newSelector);
	m_SelectorMap.insert(make_pair(NewSelector, newSelector));
	m_Count++;
}

void BehaviorTree::AddRootChildSelector()
{
	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootSequence != NULL)
	{
		assert(false);
		return;
	}

	m_RootSelector = new Selector();
	m_RootSelector->SetTag(m_RootSelectorName);
	m_RootSelector->SetTreeName(m_TagName);
	m_RootSelector->SetKeepAction(m_RootNode);

	m_RootNode->SetChild(m_RootSelector);
}

void BehaviorTree::AddRootChildSequence()
{
	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootSelector != NULL)
	{
		assert(false);
		return;
	}

	m_RootSequence = new Sequence();
	m_RootSequence->SetTag(m_RootSequenceName);
	m_RootSequence->SetTreeName(m_TagName);
	m_RootSequence->SetKeepAction(m_RootNode);

	m_RootNode->SetChild(m_RootSequence);
}

void BehaviorTree::SetObject(CGameObject * Object)
{
	m_RootNode->SetObject(Object);

	if (m_RootSequence != NULL)
		m_RootSequence->SetAllActionObject(Object);

	else if (m_RootSelector != NULL)
		m_RootSelector->SetAllActionObject(Object);
}

void BehaviorTree::Init(BT_ROOT_CHILD_TYPE eStyle)
{
	m_RootNode = new RootNode();
	m_RootNode->SetTag(m_RootName);
	m_RootNode->SetTreeName(m_TagName);

	switch (eStyle)
	{
	case BT_SELECTOR:
		AddRootChildSelector();
		break;
	case BT_SEQUENCE:
		AddRootChildSequence();
		break;
	}
}

void BehaviorTree::GUIRender()
{
	/*
	*/
}

BehaviorTree::Sequence * BehaviorTree::FindSequence(const string & KeyName)
{
	auto FindIter = m_SequenceMap.find(KeyName);

	if (FindIter == m_SequenceMap.end())
		return NULL;

	return FindIter->second;
}

BehaviorTree::Selector * BehaviorTree::FindSelector(const string & KeyName)
{
	auto FindIter = m_SelectorMap.find(KeyName);

	if (FindIter == m_SelectorMap.end())
		return NULL;

	return FindIter->second;
}

BehaviorTree::Action * BehaviorTree::FindAction(const string & KeyName)
{
	auto FindIter = m_ActionMap.find(KeyName);

	if (FindIter == m_ActionMap.end())
		return NULL;
	
	return FindIter->second;
}

void BehaviorTree::CompositNode::SetAllActionObject(CGameObject * Object)
{
	for (size_t i = 0; i < m_ChildList.size(); i++)
		m_ChildList[i]->SetObject(Object);
}

int BehaviorTree::Selector::Update(float DeltaTime)
{
	if (m_isCheck == true)
	{
		m_TimeVar += DeltaTime;

		if (m_TimeVar >= m_CheckTime)
		{
			m_TimeVar = 0.0f;
			m_TickFunc(DeltaTime);
		}
	}

	if (m_vecDecorator.empty() == true)
		return Process(DeltaTime);
	else
	{
		bool Value = false;
		for (size_t i = 0; i < m_vecDecorator.size(); i++)
		{
			Value = m_vecDecorator[i](DeltaTime);

			if (Value == false)
				return ACTION_SUCCED;
		}
		return Process(DeltaTime);
	}
}

int BehaviorTree::Selector::Process(float DeltaTime)
{
	if (m_bRandom == false)
	{
		for (size_t i = 0; i < m_ChildList.size(); i++)
		{
			BT_ACTION_TYPE type = (BT_ACTION_TYPE)m_ChildList[i]->Update(DeltaTime);
			m_ChildList[i]->SetType(type);

			switch (type)
			{
			case ACTION_SUCCED:
				m_ChildList[i]->Ending(DeltaTime);
				return ACTION_SUCCED;
				break;
			case ACTION_FALSE:
				break;
			case ACTION_RUNNING:
				BTManager::Get()->FindTree(m_TreeName)->m_CurAction = m_ChildList[i];
				return ACTION_RUNNING;
				break;
			}
		}
		return ACTION_FALSE;
	}
	else
	{
		int RandomNum = rand() % m_ChildList.size();

		BT_ACTION_TYPE type = (BT_ACTION_TYPE)m_ChildList[RandomNum]->Update(DeltaTime);
		m_ChildList[RandomNum]->SetType(type);

		switch (type)
		{
		case ACTION_SUCCED:
			m_ChildList[RandomNum]->Ending(DeltaTime);
			return ACTION_SUCCED;
			break;
		case ACTION_FALSE:
			return ACTION_FALSE;
			break;
		case ACTION_RUNNING:
			BTManager::Get()->FindTree(m_TreeName)->m_CurAction = m_ChildList[RandomNum];
			return ACTION_RUNNING;
			break;
		}
		return ACTION_FALSE;
	}
}

int BehaviorTree::Sequence::Update(float DeltaTime)
{
	if (m_isCheck == true)
	{
		m_TimeVar += DeltaTime;

		if (m_TimeVar >= m_CheckTime)
		{
			m_TimeVar = 0.0f;
			m_TickFunc(DeltaTime);
		}
	}

	if (m_vecDecorator.empty() == true)
	{
		for (size_t i = 0; i < m_ChildList.size(); i++)
		{
			BT_ACTION_TYPE type = (BT_ACTION_TYPE)m_ChildList[i]->Update(DeltaTime);
			m_ChildList[i]->SetType(type);

			switch (type)
			{
			case ACTION_RUNNING:
				BTManager::Get()->FindTree(m_TreeName)->m_CurAction = m_ChildList[i];
				return ACTION_RUNNING;
				break;
			case ACTION_FALSE:
				m_ChildList[i]->Ending(DeltaTime);
			return ACTION_FALSE;
			break;
			case ACTION_SUCCED:
				break;
			}
		}
	}
	else
	{
		bool Value = false;
		for (size_t i = 0; i < m_vecDecorator.size(); i++)
		{
			Value = m_vecDecorator[i](DeltaTime);

			if (Value == false)
				return ACTION_FALSE;
		}

		for (size_t i = 0; i < m_ChildList.size(); i++)
		{
			BT_ACTION_TYPE type = (BT_ACTION_TYPE)m_ChildList[i]->Update(DeltaTime);
			m_ChildList[i]->SetType(type);

			switch (type)
			{
			case ACTION_RUNNING:
				BTManager::Get()->FindTree(m_TreeName)->m_CurAction = m_ChildList[i];
				return ACTION_RUNNING;
				break;
			case ACTION_FALSE:
				m_ChildList[i]->Ending(DeltaTime);
			return ACTION_FALSE;
				break;
			case ACTION_SUCCED:
				break;
			}
		}
	}
	return ACTION_SUCCED;
}