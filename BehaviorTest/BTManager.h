#pragma once

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

	void Delete(const string& BTName);
	void Render(const string& BTName);

public:
	class BehaviorTree* CreateBehaviorTree(const string& KeyName, BT_ROOT_CHILD_TYPE eStyle = BT_SELECTOR);
	class BehaviorTree* FindTree(const string& KeyName);

private:
	unordered_map<string, class BehaviorTree*> m_TreeMap;
	static BTManager* m_Instance;

private:
	BTManager();
	~BTManager();
};