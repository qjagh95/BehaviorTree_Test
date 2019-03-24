#pragma once

class BehaviorTree;
class TreeManager
{
public:
	static TreeManager* Get()
	{
		if (m_Instance == NULL)
			m_Instance = new TreeManager();
		
		return m_Instance;
	}

	static void Delete()
	{
		delete m_Instance;
	}

	void Delete(const string& BTName)
	{
		m_TreeMap.erase(BTName);
	}

public:
	BehaviorTree* CreateBehaviorTree(const string& KeyName, BT_ROOT_CHILD_TYPE eStyle = BT_SEQUENCE);
	BehaviorTree* FindTree(const string& KeyName);

private:
	unordered_map<string, BehaviorTree*> m_TreeMap;
	static TreeManager* m_Instance;

private:
	TreeManager();
	~TreeManager();
};

