#pragma once

class CGameObject;
class BehaviorTree
{
public:
	class Action
	{
	public:
		virtual int Update(float DeltaTime) = 0;
		virtual void Ending(float DeltaTime) {};

		void SetTag(const string& Name) { m_TagName = Name; }
		void SetTreeName(const string& Name) { m_TreeName = Name; }
		string GetTag() const { return m_TagName; }
		string GetTreeName() const { return m_TreeName; }
		void SetObject(CGameObject* object) { m_pObject = object; }
		CGameObject* GetObject() const { return m_pObject; }
		BT_ACTION_TYPE GetType() const { return m_Type; }
		void SetType(BT_ACTION_TYPE type) { m_Type = type; }
		BT_ROOT_CHILD_TYPE GetActionType() const { return m_NodeType; }
		void SetActionType(BT_ROOT_CHILD_TYPE type) { m_NodeType = type; }
		void SetKeepActionType(BT_ROOT_CHILD_TYPE type) { m_KeepNodeType = type; }
		BT_ROOT_CHILD_TYPE GetKeepActionType() const { return m_KeepNodeType; }
		void SetKeepAction(Action* action) { m_KeepNode = action; }
		Action* GetKeepAction() const { return m_KeepNode; }

	protected:
		Action() { m_KeepNode = NULL; m_pObject = NULL; m_Type = BAT_NONE; m_NodeType = BT_NONE; m_KeepNodeType = BT_NONE; }
		string m_TagName;
		string m_TreeName;
		BT_ACTION_TYPE m_Type;
		BT_ROOT_CHILD_TYPE m_NodeType;
		BT_ROOT_CHILD_TYPE m_KeepNodeType;
		Action* m_KeepNode;

	private:
		CGameObject* m_pObject;
	};

	class CompositNode : public Action
	{
	public:
		void AddChild(Action* Node) { m_ChildList.push_back(Node); }
		vector<Action*>* GetChildList() { return &m_ChildList; }
		vector<Action*>* ChildShuffied() { random_shuffle(m_ChildList.begin(), m_ChildList.end()); }

	protected:
		void SetAllActionObject(CGameObject* Object);

	protected:
		vector<Action*> m_ChildList;

		CompositNode() {}
		virtual ~CompositNode()
		{
			for (size_t i = 0; i < m_ChildList.size(); i++)
				delete m_ChildList[i];
		}
	};

	class Selector : public CompositNode
	{
	public:
		virtual int Update(float DeltaTime) override;
		void SetRandomProcess(bool Value) { m_bRandom = Value; }

	private:
		Selector() { m_bRandom = false; }
		bool m_bRandom;

	public:
		friend class BehaviorTree;
	};

	class Sequence : public CompositNode
	{
	public:
		virtual int Update(float DeltaTime) override;

	public:
		friend class BehaviorTree;
	};

	class Decorator : public Action
	{
	public:
		void SetChild(Action* newChild) { m_ChildNode = newChild; }
		virtual int Update(float DeltaTime) override;
		Action* GetChild() const { return m_ChildNode; }

	protected:
		Action* m_ChildNode;
		function<int(float)> m_CheckFunc;

	protected:
		friend class BehaviorTree;
		Decorator() { m_ChildNode = NULL; }
	};

	class RootNode : public Action
	{
	public:
		void SetChild(Action* newChild) { m_ChildNode = newChild; }

		int Update(float DeltaTime) override
		{
			return m_ChildNode->Update(DeltaTime);
		}
		void Ending(float DeltaTime) override
		{
			return m_ChildNode->Ending(DeltaTime);
		}

	private:
		Action* m_ChildNode;

	public:
		friend class BehaviorTree;
	};

public:
	void Update(float DeltaTime);

	void AddRootSequenceInSequence(const string& NewSequenceKeyName);
	void AddRootSequenceInSelector(const string& NewSelectorKeyName);
	void AddRootSequenceInDecorator(const string& NewDecoratorKeyName);
	void AddRootSequenceInAction(const string& SequenceKeyName, Action* NewAction);

	void AddRootSelectorInSequence(const string& NewSequenceKeyName);
	void AddRootSelectorInSelector(const string& NewSelectorKeyName);
	void AddRootSelectorInDecorator(const string& NewDecoratorKeyName);
	void AddRootSelectorInAction(const string& SelectorKeyName, Action* NewAction);

	void AddRootDecoratorInAction(const string& DecoratorKeyName, Action* NewAction);
	
	void SetDecoratorCheckFunc(const string& DecoratorKeyName, bool(*pFunc)(float))
	{
		Decorator* getDecorator = FindDecorator(DecoratorKeyName);

		if (getDecorator == NULL)
		{
			assert(true);
			return;
		}

		getDecorator->m_CheckFunc = bind(pFunc, placeholders::_1);
	}

	template<typename T>
	void SetDecoratorCheckFunc(const string& DecoratorKeyName, T* object, int(T::*pFunc)(float))
	{
		Decorator* getDecorator = FindDecorator(DecoratorKeyName);

		if (getDecorator == NULL)
		{
			assert(true);
			return;
		}

		getDecorator->m_CheckFunc = bind(pFunc, object, placeholders::_1);
	}

	void AddSequenceInAction(const string& SequenceKeyName, Action* NewAction);
	void AddSelectorInAction(const string& SelectorKeyName, Action* NewAction);
	void AddDecoratorInAction(const string& DecoratorKeyName, Action* NewAction);

	void AddSequenceInSelector(const string& SequenceKeyName, const string& SelectorKeyName);
	void AddSequenceInSequence(const string& OldSequenceKey, const string& NewSequenceKey);
	void AddSequenceInDecorator(const string& SequenceKeyName, const string& DecoratorKeyName);
	
	void AddSelectorInSelector(const string& OldSelectorKey, const string& NewSelector);
	void AddSelectorInSequence(const string& SelectorKeyName, const string& SequenceKeyName);
	void AddSelectorInDecorator(const string& SelectorKeyName, const string& DecoratorKeyName);

	void SetSelectorRandomProcess(const string& SelectorKeyName, bool Value);

	Sequence* GetRootSequence() const { return m_RootSequence; }
	RootNode* GetRootNode() const { return m_RootNode; }

	CGameObject* GetObject() const { return m_RootNode->GetObject(); }
	void SetAllActionObject(CGameObject* Object);

private:
	void Init(BT_ROOT_CHILD_TYPE eStyle = BT_SEQUENCE);

	Sequence* FindSequence(const string& KeyName);
	Selector* FindSelector(const string& KeyName);
	Decorator* FindDecorator(const string& KeyName);

	void AddRootChildSelector();
	void AddRootChildSequence();
	void AddRootChildDecorator();

private:
	RootNode* m_RootNode;
	Sequence* m_RootSequence;
	Selector* m_RootSelector;
	Decorator* m_RootDecorator;

	unordered_map<string, Sequence*> m_SequenceMap;
	unordered_map<string, Selector*> m_SelectorMap;
	unordered_map<string, Decorator*> m_DecoratorMap;
	vector<Action*> m_vecAction;

	Action* m_CurAction;
	string m_TagName;
	bool m_isOnePath;
	BT_ACTION_TYPE m_Type;

private:
	BehaviorTree();
	~BehaviorTree();

	friend class TreeManager;
};