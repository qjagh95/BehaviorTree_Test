#include "pch.h"
#include "BehaviorTree.h"
#include "TreeManager.h"

BehaviorTree::BehaviorTree()
{
	m_RootNode = NULL;
	m_RootSequence = NULL;
	m_RootSelector = NULL;
	m_RootDecorator = NULL;
	m_CurAction = NULL;
	m_isOnePath = false;
	m_Type = BAT_NONE;
}

BehaviorTree::~BehaviorTree()
{
	unordered_map<string, Sequence*>::iterator StartIter = m_SequenceMap.begin();
	unordered_map<string, Sequence*>::iterator EndIter = m_SequenceMap.end();

	for (; StartIter != EndIter; StartIter++)
		delete StartIter->second;

	unordered_map<string, Selector*>::iterator StartIter1 = m_SelectorMap.begin();
	unordered_map<string, Selector*>::iterator EndIter1 = m_SelectorMap.end();

	for (; StartIter1 != EndIter1; StartIter1++)
		delete StartIter1->second;

	unordered_map<string, Decorator*>::iterator StartIter2 = m_DecoratorMap.begin();
	unordered_map<string, Decorator*>::iterator EndIter2 = m_DecoratorMap.end();

	for (; StartIter2 != EndIter2; StartIter2++)
		delete StartIter2->second;

	for (size_t i = 0; i < m_vecAction.size(); i++)
		delete m_vecAction[i];
}

void BehaviorTree::Update(float DeltaTime)
{
	switch (m_Type)
	{
	case BAT_RUNNING:
		switch (m_CurAction->GetKeepActionType())
		{
		case BT_NONE:
		case BT_ROOT:
		case BT_SELECTOR:
		case BT_SEQUENCE:
			m_CurAction->Update(DeltaTime);
			break;

		case BT_DECORATOR:
		{
			Decorator* getParent = (Decorator*)m_CurAction->GetKeepAction();
				
			if(getParent->m_CheckFunc(DeltaTime) == BAT_SUCCED)
				m_CurAction->Update(DeltaTime);
		}
			break;

		}
		break;
	case BAT_NONE:
	case BAT_SUCCED:
	case BAT_FALSE:
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
		newSequence->SetActionType(BT_SEQUENCE);
		newSequence->SetKeepActionType(BT_SEQUENCE);
		newSequence->SetKeepAction(m_RootSequence);

		m_RootSequence->AddChild(newSequence);
		m_SequenceMap.insert(make_pair(NewSequenceKeyName, newSequence));
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
	newSelector->SetActionType(BT_SELECTOR);
	newSelector->SetKeepActionType(BT_SEQUENCE);
	newSelector->SetKeepAction(m_RootSequence);

	m_RootSequence->AddChild(newSelector);
	m_SelectorMap.insert(make_pair(NewSelectorKeyName, newSelector));
}

void BehaviorTree::AddRootSequenceInDecorator(const string & NewDecoratorKeyName)
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

	Decorator* newDecorator = FindDecorator(NewDecoratorKeyName);

	if (newDecorator != NULL)
	{
		assert(false);
		return;
	}

	newDecorator = new Decorator();
	newDecorator->SetTag(NewDecoratorKeyName);
	newDecorator->SetTreeName(m_TagName);
	newDecorator->SetActionType(BT_DECORATOR);
	newDecorator->SetKeepActionType(BT_SEQUENCE);
	newDecorator->SetKeepAction(m_RootSequence);

	m_RootSequence->AddChild(newDecorator);

	m_DecoratorMap.insert(make_pair(NewDecoratorKeyName, newDecorator));
}

void BehaviorTree::AddRootSequenceInAction(const string& SequenceKeyName, Action* NewAction)
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

	NewAction->SetTag(SequenceKeyName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetActionType(BT_ACTION);
	NewAction->SetKeepActionType(BT_SEQUENCE);
	NewAction->SetKeepAction(m_RootSequence);

	m_RootSequence->AddChild(NewAction);
	m_vecAction.push_back(NewAction);
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
	newSequence->SetTag(NewSequenceKeyName);
	newSequence->SetTreeName(m_TagName);
	newSequence->SetActionType(BT_SEQUENCE);
	newSequence->SetKeepActionType(BT_SELECTOR);
	newSequence->SetKeepAction(m_RootSelector);

	m_RootSelector->AddChild(newSequence);
	m_SequenceMap.insert(make_pair(NewSequenceKeyName, newSequence));
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
	newSelector->SetTag(NewSelectorKeyName);
	newSelector->SetTreeName(m_TagName);
	newSelector->SetActionType(BT_SELECTOR);
	newSelector->SetKeepActionType(BT_SELECTOR);
	newSelector->SetKeepAction(m_RootSelector);

	m_RootSelector->AddChild(newSelector);
	m_SelectorMap.insert(make_pair(NewSelectorKeyName, newSelector));
}

void BehaviorTree::AddRootSelectorInDecorator(const string & NewDecoratorKeyName)
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

	Decorator* newDecorator = new Decorator();
	newDecorator->SetTag(NewDecoratorKeyName);
	newDecorator->SetTreeName(m_TagName);
	newDecorator->SetActionType(BT_DECORATOR);
	newDecorator->SetKeepActionType(BT_SELECTOR);
	newDecorator->SetKeepAction(m_RootSelector);

	m_RootSelector->AddChild(newDecorator);
	m_DecoratorMap.insert(make_pair(NewDecoratorKeyName, newDecorator));
}

void BehaviorTree::AddRootSelectorInAction(const string & SelectorKeyName, Action * NewAction)
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

	m_RootSelector->AddChild(NewAction);
	NewAction->SetTag(SelectorKeyName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetActionType(BT_ACTION);
	NewAction->SetKeepActionType(BT_SELECTOR);
	NewAction->SetKeepAction(m_RootSelector);

	m_vecAction.push_back(NewAction);
}

void BehaviorTree::AddRootDecoratorInAction(const string & DecoratorKeyName, Action * NewAction)
{
	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootDecorator == NULL)
	{
		assert(false);
		return;
	}

	m_RootDecorator->SetChild(NewAction);
	NewAction->SetTag(DecoratorKeyName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetActionType(BT_ACTION);
	NewAction->SetKeepActionType(BT_DECORATOR);
	NewAction->SetKeepAction(m_RootDecorator);

	m_vecAction.push_back(NewAction);
}

void BehaviorTree::AddSequenceInAction(const string & SequenceKeyName, Action* NewAction)
{
	Sequence* getSequence = FindSequence(SequenceKeyName);

	if (getSequence == NULL)
	{
		assert(false);
		return;
	}

	getSequence->AddChild(NewAction);
	NewAction->SetTag(SequenceKeyName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetActionType(BT_ACTION);
	NewAction->SetKeepActionType(BT_SEQUENCE);
	NewAction->SetKeepAction(getSequence);

	m_vecAction.push_back(NewAction);
}

void BehaviorTree::AddSelectorInAction(const string & SelectorKeyName, Action * NewAction)
{
	Selector* getSelector = FindSelector(SelectorKeyName);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	getSelector->AddChild(NewAction);

	NewAction->SetTag(SelectorKeyName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetActionType(BT_ACTION);
	NewAction->SetKeepActionType(BT_SEQUENCE);
	NewAction->SetKeepAction(getSelector);

	m_vecAction.push_back(NewAction);
}

void BehaviorTree::AddDecoratorInAction(const string & DecoratorKeyName, Action * NewAction)
{
	Decorator* getDecorator = FindDecorator(DecoratorKeyName);

	if (getDecorator == NULL)
	{
		assert(false);
		return;
	}

	getDecorator->SetChild(NewAction);
	NewAction->SetTag(DecoratorKeyName);
	NewAction->SetTreeName(m_TagName);
	NewAction->SetActionType(BT_ACTION);
	NewAction->SetKeepActionType(BT_DECORATOR);
	NewAction->SetKeepAction(getDecorator);

	m_vecAction.push_back(NewAction);
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
	newSelector->SetTag(SelectorKeyName);
	newSelector->SetTreeName(m_TagName);
	newSelector->SetActionType(BT_SELECTOR);
	newSelector->SetKeepActionType(BT_SEQUENCE);
	newSelector->SetKeepAction(getSequence);

	getSequence->AddChild(newSelector);
	m_SelectorMap.insert(make_pair(SelectorKeyName, newSelector));
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
	newSequence->SetTag(SequenceKeyName);
	newSequence->SetTreeName(m_TagName);
	newSequence->SetActionType(BT_SEQUENCE);
	newSequence->SetKeepActionType(BT_SELECTOR);
	newSequence->SetKeepAction(getSelector);

	getSelector->AddChild(newSequence);
	m_SequenceMap.insert(make_pair(SequenceKeyName, newSequence));
}

void BehaviorTree::AddSelectorInDecorator(const string & SelectorKeyName, const string & DecoratorKeyName)
{
	Selector* getSelector = FindSelector(SelectorKeyName);

	if (getSelector == NULL)
	{
		assert(false);
		return;
	}

	Decorator* newDecorator = FindDecorator(DecoratorKeyName);

	if (newDecorator != NULL)
	{
		assert(false);
		return;
	}

	newDecorator = new Decorator();
	newDecorator->SetTag(DecoratorKeyName);
	newDecorator->SetTreeName(m_TagName);
	newDecorator->SetActionType(BT_DECORATOR);
	newDecorator->SetKeepActionType(BT_SELECTOR);
	newDecorator->SetKeepAction(getSelector);

	getSelector->AddChild(newDecorator);

	m_DecoratorMap.insert(make_pair(DecoratorKeyName, newDecorator));
}

void BehaviorTree::AddSequenceInSequence(const string & OldSequenceKey, const string & NewSequenceKey)
{
	Sequence* getSelector = FindSequence(OldSequenceKey);

	if(getSelector == NULL)
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
	newSequence->SetTag(NewSequenceKey);
	newSequence->SetTreeName(m_TagName);
	newSequence->SetActionType(BT_SEQUENCE);
	newSequence->SetKeepActionType(BT_SEQUENCE);
	newSequence->SetKeepAction(getSelector);

	getSelector->AddChild(newSequence);
	m_SequenceMap.insert(make_pair(NewSequenceKey, newSequence));
}

void BehaviorTree::AddSequenceInDecorator(const string & SequenceKeyName, const string & DecoratorKeyName)
{
	Sequence* getSequence = FindSequence(SequenceKeyName);

	if (getSequence == NULL)
	{
		assert(false);
		return;
	}

	Decorator* newDecorator = FindDecorator(DecoratorKeyName);

	if (newDecorator != NULL)
	{
		assert(false);
		return;
	}

	newDecorator = new Decorator();
	newDecorator->SetTag(DecoratorKeyName);
	newDecorator->SetTreeName(m_TagName);
	newDecorator->SetActionType(BT_DECORATOR);
	newDecorator->SetKeepActionType(BT_SEQUENCE);
	newDecorator->SetKeepAction(getSequence);

	getSequence->AddChild(newDecorator);
	m_DecoratorMap.insert(make_pair(DecoratorKeyName, newDecorator));
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
	newSelector->SetTag(NewSelector);
	newSelector->SetTreeName(m_TagName);
	newSelector->SetActionType(BT_SELECTOR);
	newSelector->SetKeepActionType(BT_SELECTOR);
	newSelector->SetKeepAction(getSelector);

	getSelector->AddChild(newSelector);
	m_SelectorMap.insert(make_pair(NewSelector, newSelector));
}

void BehaviorTree::AddRootChildSelector()
{
	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootSequence != NULL || m_RootDecorator != NULL)
	{
		assert(false);
		return;
	}

	m_RootSelector = new Selector();
	m_RootSelector->SetTag("RootSelector");
	m_RootSelector->SetTreeName(m_TagName);
	m_RootSelector->SetActionType(BT_SELECTOR);
	m_RootSelector->SetKeepActionType(BT_ROOT);
	m_RootSelector->SetKeepAction(m_RootNode);

	m_SelectorMap.insert(make_pair("RootSelector", m_RootSelector));
	m_RootNode->SetChild(m_RootSelector);
}

void BehaviorTree::AddRootChildSequence()
{
	if(m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootSelector != NULL || m_RootDecorator != NULL)
	{
		assert(false);
		return;
	}

	m_RootSequence = new Sequence();
	m_RootSequence->SetTag("RootSequence");
	m_RootSequence->SetTreeName(m_TagName);
	m_RootSequence->SetActionType(BT_SEQUENCE);
	m_RootSequence->SetKeepActionType(BT_ROOT);
	m_RootSequence->SetKeepAction(m_RootNode);

	m_SequenceMap.insert(make_pair("RootSequence", m_RootSequence));
	m_RootNode->SetChild(m_RootSequence);
}

void BehaviorTree::AddRootChildDecorator()
{
	if (m_RootNode == NULL)
	{
		assert(false);
		return;
	}

	if (m_RootSelector != NULL || m_RootSequence != NULL)
	{
		assert(false);
		return;
	}

	m_RootDecorator = new Decorator();
	m_RootDecorator->SetTag("RootDecorator");
	m_RootDecorator->SetActionType(BT_DECORATOR);
	m_RootDecorator->SetKeepActionType(BT_ROOT);
	m_RootDecorator->SetKeepAction(m_RootNode);

	m_DecoratorMap.insert(make_pair("RootDecorator", m_RootDecorator));
	m_RootNode->SetChild(m_RootDecorator);
}

void BehaviorTree::SetAllActionObject(CGameObject * Object)
{
	m_RootNode->SetObject(Object);

	if (m_RootSequence != NULL)
		m_RootSequence->SetAllActionObject(Object);

	else if (m_RootSelector != NULL)
		m_RootSelector->SetAllActionObject(Object);

	else if (m_RootDecorator != NULL)
		m_RootDecorator->SetObject(Object);
}

void BehaviorTree::Init(BT_ROOT_CHILD_TYPE eStyle)
{
	m_RootNode = new RootNode();
	m_RootNode->SetTag("RootNode");
	m_RootNode->SetKeepActionType(BT_NONE);
	m_RootNode->SetActionType(BT_ROOT);
	m_RootNode->SetTreeName(m_TagName);

	switch (eStyle)
	{
	case BT_SELECTOR:
		AddRootChildSelector();
		break;
	case BT_SEQUENCE:
		AddRootChildSequence();
		break;
	case BT_DECORATOR:
		AddRootChildDecorator();
		break;
	}
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

BehaviorTree::Decorator * BehaviorTree::FindDecorator(const string & KeyName)
{
	auto FindIter = m_DecoratorMap.find(KeyName);

	if (FindIter == m_DecoratorMap.end())
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
	if (m_bRandom == false)
	{
		for (size_t i = 0; i < m_ChildList.size(); i++)
		{
			BT_ACTION_TYPE type = (BT_ACTION_TYPE)m_ChildList[i]->Update(DeltaTime);
			m_ChildList[i]->SetType(type);

			switch (type)
			{
			case BAT_SUCCED:
				m_ChildList[i]->Ending(DeltaTime);
				return BAT_SUCCED;
				break;
			case BAT_FALSE:
				break;
			case BAT_RUNNING:
				Action* getAction = TreeManager::Get()->FindTree(m_TreeName)->m_CurAction;

				if (getAction != NULL && getAction->GetType() == BAT_RUNNING)
					return BAT_NONE;
				else
				{
					TreeManager::Get()->FindTree(m_TreeName)->m_CurAction = m_ChildList[i];
					return BAT_RUNNING;
				}
				return BAT_RUNNING;
				break;
			}
		}
		return BAT_FALSE;
	}
	else
	{
		int RandomNum = rand() % m_ChildList.size();

		BT_ACTION_TYPE type = (BT_ACTION_TYPE)m_ChildList[RandomNum]->Update(DeltaTime);
		m_ChildList[RandomNum]->SetType(type);

		switch (type)
		{
		case BAT_SUCCED:
			m_ChildList[RandomNum]->Ending(DeltaTime);
			return BAT_SUCCED;
			break;
		case BAT_FALSE:
			return BAT_FALSE;
			break;
		case BAT_RUNNING:
		{	
			Action* getAction = TreeManager::Get()->FindTree(m_TreeName)->m_CurAction;

			if (getAction != NULL && getAction->GetType() != BAT_RUNNING)
				return BAT_NONE;
			else
			{
				TreeManager::Get()->FindTree(m_TreeName)->m_CurAction = m_ChildList[RandomNum];
				return BAT_RUNNING;
			}
		}
			break;
		}
		return BAT_FALSE;
	}
}

int BehaviorTree::Sequence::Update(float DeltaTime)
{
	for (size_t i = 0; i < m_ChildList.size(); i++)
	{
		BT_ACTION_TYPE type = (BT_ACTION_TYPE)m_ChildList[i]->Update(DeltaTime);
		m_ChildList[i]->SetType(type);

		switch (type)
		{
		case BAT_RUNNING:
		{
			Action* getAction = TreeManager::Get()->FindTree(m_TreeName)->m_CurAction;

			if (getAction != NULL && getAction->GetType() != BAT_RUNNING)
				return BAT_NONE;
			else
			{
				TreeManager::Get()->FindTree(m_TreeName)->m_CurAction = m_ChildList[i];
				return BAT_RUNNING;
			}
		}
		case BAT_FALSE:
			m_ChildList[i]->Ending(DeltaTime);
			return BAT_FALSE;
			break;
		case BAT_SUCCED:
			break;
		}
	}

	return BAT_SUCCED;
}

int BehaviorTree::Decorator::Update(float DeltaTime)
{
	int Value = m_CheckFunc(DeltaTime);

	if (Value == BAT_SUCCED)
	{
		BT_ACTION_TYPE type = (BT_ACTION_TYPE)m_ChildNode->Update(DeltaTime);
		m_ChildNode->SetType(type);

		switch (type)
		{
		case BAT_RUNNING:
		{
			Action* getAction = TreeManager::Get()->FindTree(m_TreeName)->m_CurAction;

			if (getAction != NULL && getAction->GetType() != BAT_RUNNING)
				return BAT_NONE;
			else
			{
				TreeManager::Get()->FindTree(m_TreeName)->m_CurAction = m_ChildNode;
				return BAT_RUNNING;
			}
			break;
		}}
		return BAT_SUCCED;
	}
	else if(Value == BAT_RUNNING)
		return BAT_RUNNING;

	return BAT_FALSE;
}
