#include "pch.h"
#include "BTManager.h"
#include "BehaviorTree.h"

BTManager* BTManager::m_Instance = NULL;

BTManager::BTManager()
{
}

BTManager::~BTManager()
{
	unordered_map<string, BehaviorTree*>::iterator StartIter = m_TreeMap.begin();
	unordered_map<string, BehaviorTree*>::iterator EndIter = m_TreeMap.end();
	
	for (;StartIter != EndIter; StartIter++)
		delete StartIter->second;
}

void BTManager::Delete(const string & BTName)
{
	m_TreeMap.erase(BTName);
}

void BTManager::Render(const string & BTName)
{
}

BehaviorTree* BTManager::CreateBehaviorTree(const string& KeyName, BT_ROOT_CHILD_TYPE eStyle)
{
	BehaviorTree* newTree = new BehaviorTree();
	newTree->m_TagName = KeyName;
	newTree->Init(eStyle);

	m_TreeMap.insert(make_pair(KeyName, newTree));

	return newTree;
}

BehaviorTree * BTManager::FindTree(const string & KeyName)
{
	auto FindIter = m_TreeMap.find(KeyName);

	if (FindIter == m_TreeMap.end())
		return NULL;
	
	return FindIter->second;
}