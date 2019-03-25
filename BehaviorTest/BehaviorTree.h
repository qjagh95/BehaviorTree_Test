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
		Action() { m_KeepNode = NULL; m_pObject = NULL; m_Type = ACTION_NONE; m_NodeType = BT_NONE; m_KeepNodeType = BT_NONE; }

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
		virtual ~CompositNode()	{}
	};

	class Selector : public CompositNode
	{
	public:
		virtual int Update(float DeltaTime) override;
		void SetRandomProcess(bool Value) { m_bRandom = Value; }

		void AddDecorator(bool(*pFunc)(float))
		{
			m_vecDecorator.push_back(bind(pFunc, placeholders::_1));
		}

		template<typename T>
		void AddDecorator(T* object, bool(T::*pFunc)(float))
		{
			m_vecDecorator.push_back(bind(pFunc,object, placeholders::_1));
		}

		void AddTickFunc(float CallbackTime, void(*pFunc)(float))
		{
			m_isCheck = true;
			m_CheckTime = CallbackTime;
			m_TickFunc = bind(pFunc, placeholders::_1);
		}

		template<typename T>
		void AddTickFunc(float CallbackTime, T* object, void(T::*pFunc)(float))
		{
			m_isCheck = true
			m_CheckTime = CallbackTime;
			m_TickFunc = bind(pFunc, object, placeholders::_1);
		}

	private:
		int Process(float DeltaTime);
		Selector() { m_bRandom = false; m_vecDecorator.reserve(4); m_TimeVar = 0.0f; m_CheckTime = 0.0f; m_isCheck = false; }
		~Selector() {}

	private:
		bool m_bRandom;
		vector<function<bool(float)>> m_vecDecorator;
		function<void(float)> m_TickFunc;
		float m_TimeVar;
		float m_CheckTime;
		bool m_isCheck;

	public:
		friend class BehaviorTree;
	};

	class Sequence : public CompositNode
	{
	public:
		virtual int Update(float DeltaTime) override;

		void AddDecorator(bool(*pFunc)(float))
		{
			m_vecDecorator.push_back(bind(pFunc, placeholders::_1));
		}

		template<typename T>
		void AddDecorator(T* object, bool(T::*pFunc)(float))
		{
			m_vecDecorator.push_back(bind(pFunc, object, placeholders::_1));
		}

		void AddTickFunc(float CallbackTime, void(*pFunc)(float))
		{
			m_isCheck = true;
			m_CheckTime = CallbackTime;
			m_TickFunc = bind(pFunc, placeholders::_1);
		}

		template<typename T>
		void AddTickFunc(float CallbackTime, T* object, void(T::*pFunc)(float))
		{
			m_isCheck = true;
			m_CheckTime = CallbackTime;
			m_TickFunc = bind(pFunc, object, placeholders::_1);
		}

	private:
		Sequence() { m_vecDecorator.reserve(4);	m_TimeVar = 0.0f; m_CheckTime = 0.0f; }
		~Sequence() {}

		vector<function<bool(float)>> m_vecDecorator;
		function<void(float)> m_TickFunc;
		float m_TimeVar;
		float m_CheckTime;
		bool m_isCheck; 

	public:
		friend class BehaviorTree;

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
		~RootNode() {}

	public:
		friend class BehaviorTree;
	};

public:
	void Update(float DeltaTime);

	void AddRootSequenceInSequence(const string& NewSequenceKeyName);
	void AddRootSequenceInSelector(const string& NewSelectorKeyName);
	void AddRootSequenceInAction(const string& SequenceKeyName, Action* NewAction);
	void AddRootSelectorInSequence(const string& NewSequenceKeyName);
	void AddRootSelectorInSelector(const string& NewSelectorKeyName);
	void AddRootSelectorInAction(const string& SelectorKeyName, Action* NewAction);
	void AddSequenceInAction(const string& SequenceKeyName, Action* NewAction);
	void AddSelectorInAction(const string& SelectorKeyName, Action* NewAction);
	void AddSequenceInSelector(const string& SequenceKeyName, const string& SelectorKeyName);
	void AddSequenceInSequence(const string& OldSequenceKey, const string& NewSequenceKey);
	void AddSelectorInSelector(const string& OldSelectorKey, const string& NewSelector);
	void AddSelectorInSequence(const string& SelectorKeyName, const string& SequenceKeyName);
	void SetSelectorRandomProcess(const string& SelectorKeyName, bool Value);

	void AddSquenceInDecorator(const string& SequenceKeyName, bool(*pFunc)(float))
	{
		Sequence* getSquence = FindSequence(SequenceKeyName);

		if (getSquence == NULL)
		{
			assert(false);
			false;
		}

		getSquence->AddDecorator(pFunc);
	}

	template<typename T>
	void AddSquenceInDecorator(const string& SequenceKeyName, T* object, bool(T::*pFunc)(float))
	{
		Sequence* getSquence = FindSequence(SequenceKeyName);

		if (getSquence == NULL)
		{
			assert(false);
			false;
		}

		getSquence->AddDecorator(object, pFunc);
	}

	void AddSelectorInDecorator(const string& SelectorKeyName, bool(*pFunc)(float))
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULL)
		{
			assert(false);
			false;
		}

		getSelector->AddDecorator(pFunc);
	}

	template<typename T>
	void AddSelectorInDecorator(const string& SelectorKeyName, T* object, bool(T::*pFunc)(float))
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULL)
		{
			assert(false);
			false;
		}

		getSelector->AddDecorator(object, pFunc);
	}

	void AddSelectorInTickFunc(const string& SelectorKeyName, float CallbackTime, void(*pFunc)(float))
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULL)
		{
			assert(false);
			false;
		}

		getSelector->AddTickFunc(CallbackTime, pFunc);
	}

	template<typename T>
	void AddSelectorInTickFunc(const string& SelectorKeyName, float CallbackTime, T* object, void(T::*pFunc)(float))
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULL)
		{
			assert(false);
			false;
		}

		getSelector->AddTickFunc(CallbackTime, object, pFunc);
	}

	void AddSequenceInTickFunc(const string& SequenceKeyName, float CallbackTime, void(*pFunc)(float))
	{
		Sequence* getSequence = FindSequence(SequenceKeyName);

		if (getSequence == NULL)
		{
			assert(false);
			false;
		}

		getSequence->AddTickFunc(CallbackTime, pFunc);
	}

	template<typename T>
	void AddSequenceInTickFunc(const string& SequenceKeyName, float CallbackTime, T* object, void(T::*pFunc)(float))
	{
		Sequence* getSequence = FindSequence(SequenceKeyName);

		if (getSequence == NULL)
		{
			assert(false);
			false;
		}

		getSequence->AddTickFunc(CallbackTime, object, pFunc);
	}

	CGameObject* GetObject() const { return m_RootNode->GetObject(); }
	void SetObject(CGameObject* Object);
	string GetRootName() const { return m_RootName; }
	string GetRootSequenceName() const { return m_RootSequenceName; }
	string GetRootSelectorName() const { return m_RootSelectorName; }

private:
	void Init(BT_ROOT_CHILD_TYPE eStyle = BT_SEQUENCE);

	Sequence* FindSequence(const string& KeyName);
	Selector* FindSelector(const string& KeyName);

	void AddRootChildSelector();
	void AddRootChildSequence();

private:
	RootNode* m_RootNode;
	Sequence* m_RootSequence;
	Selector* m_RootSelector;

	unordered_map<string, Sequence*> m_SequenceMap;
	unordered_map<string, Selector*> m_SelectorMap;
	vector<Action*> m_vecAction;

	Action* m_CurAction;
	string m_TagName;
	bool m_isOnePath;
	BT_ACTION_TYPE m_Type;

	string m_RootName;
	string m_RootSequenceName;
	string m_RootSelectorName;

private:
	BehaviorTree();

public:
	~BehaviorTree();

	friend class BTManager;
};