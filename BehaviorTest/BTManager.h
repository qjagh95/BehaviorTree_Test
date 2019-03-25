#pragma once

class BehaviorTree;
class BTManager
{
public:
	static BTManager* Get()
	{
		if (m_Instance == NULL)
			m_Instance = new BTManager();
		
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
	BehaviorTree* CreateBehaviorTree(const string& KeyName, BT_ROOT_CHILD_TYPE eStyle = BT_SELECTOR);
	BehaviorTree* FindTree(const string& KeyName);

private:
	unordered_map<string, BehaviorTree*> m_TreeMap;
	static BTManager* m_Instance;

private:
	BTManager();
	~BTManager();
};

