#include "pch.h"
#include "TreeManager.h"
#include "BehaviorTree.h"

TreeManager* TreeManager::m_Instance = NULL;

TreeManager::TreeManager()
{
}

TreeManager::~TreeManager()
{
	unordered_map<string, BehaviorTree*>::iterator StartIter = m_TreeMap.begin();
	unordered_map<string, BehaviorTree*>::iterator EndIter = m_TreeMap.end();
	
	for (;StartIter != EndIter; StartIter++)
		delete StartIter->second;
}

BehaviorTree* TreeManager::CreateBehaviorTree(const string& KeyName, BT_ROOT_CHILD_TYPE eStyle)
{
	BehaviorTree* newTree = new BehaviorTree();
	newTree->m_TagName = KeyName;
	newTree->Init(eStyle);

	m_TreeMap.insert(make_pair(KeyName, newTree));

	return newTree;
}

BehaviorTree * TreeManager::FindTree(const string & KeyName)
{
	auto FindIter = m_TreeMap.find(KeyName);

	if (FindIter == m_TreeMap.end())
		return NULL;
	
	return FindIter->second;
}