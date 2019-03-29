#pragma once

class CGameObject;
class BehaviorTree
{
public:
	class Action
	{
	public:
		virtual void Init() {};
		virtual int Update(float DeltaTime) = 0;
		virtual void Ending(float DeltaTime) {};

		void SetTag(const string& Name) { m_TagName = Name; }
		void SetTreeName(const string& Name) { m_TreeName = Name; }
		string GetTag() const { return m_TagName; }
		string GetTreeName() const { return m_TreeName; }
		void SetObject(CGameObject* object) { m_pObject = object; }
		CGameObject* GetTargetObject() const { return m_pObject; }
		BT_ACTION_TYPE GetType() const { return m_Type; }
		void SetType(BT_ACTION_TYPE type) { m_Type = type; }
		void SetKeepAction(Action* action) { m_KeepNode = action; }
		Action* GetKeepAction() const { return m_KeepNode; }
		void SetKeepActionType(BT_ROOT_CHILD_TYPE type) { m_KeepNodeType = type; }
		BT_ROOT_CHILD_TYPE GetKeepActionType() const { return m_KeepNodeType; }
		vector<function<bool(float)>>* GetDecoratorVec() { return &m_vecDecorator; }
		void SetActionType(BT_ROOT_CHILD_TYPE type) { m_ActionType = type; }
		BT_ROOT_CHILD_TYPE GetActionType() const { return m_ActionType; }

		void AddDecorator(bool(*pFunc)(float))
		{
			m_vecDecorator.push_back(bind(pFunc, placeholders::_1));
		}

		template<typename T>
		void AddDecorator(T* object, bool(T::*pFunc)(float))
		{
			m_vecDecorator.push_back(bind(pFunc, object, placeholders::_1));
		}

	protected:

		Action() { m_KeepNode = NULLPTR; m_pObject = NULLPTR; m_Type = ACTION_NONE; m_ActionType = BT_NONE; }

		vector<function<bool(float)>> m_vecDecorator;
		string m_TagName;
		string m_TreeName;
		BT_ACTION_TYPE m_Type;
		BT_ROOT_CHILD_TYPE m_ActionType;
		BT_ROOT_CHILD_TYPE m_KeepNodeType;
		Action* m_KeepNode;

	private:
		CGameObject* m_pObject;

	public:
		friend class Sequence;
		friend class Selector;
	};

	class CompositNode : public Action
	{
	public:
		void AddChildAction(Action* action) { m_ChildAction.push_back(action); }
		void AddChildNode(CompositNode* Node) { m_ChildAction.push_back(Node); }

		vector<Action*>* GetChildActionList() { return &m_ChildAction; }
		vector<CompositNode*>* GetChildNodeList() { return &m_ChildNode; }

	protected:
		void SetAllActionObject(CGameObject* Object);

	protected:
		vector<Action*> m_ChildAction;
		vector<CompositNode*> m_ChildNode;

		CompositNode() {}
		virtual ~CompositNode() {}
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

		void DisableTickFunc() { m_isCheck = false; }
		void EnableTickFunc() { m_isCheck = true; }

		vector<function<bool(float)>>* GetDecorator() { return &m_vecDecorator; }

	private:
		int Process(float DeltaTime);

	private:
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

		void DisableTickFunc() { m_isCheck = false; }
		void EnableTickFunc() { m_isCheck = true; }
		vector<function<bool(float)>>* GetDecorator() { return &m_vecDecorator; }

	private:
		Sequence() { m_vecDecorator.reserve(4);	m_TimeVar = 0.0f; m_CheckTime = 0.0f; m_isCheck = false; }
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

	private:
		~RootNode() {}

	public:
		friend class BehaviorTree;
	};

public:
	void Update(float DeltaTime);

	template<typename T>
	T* AddRootSequenceInAction(const string& ActionName)
	{
		if (m_RootSequence == NULLPTR)
		{
			assert(false);
			return NULLPTR;
		}

		if (m_RootNode == NULLPTR)
		{
			assert(false);
			return NULLPTR;
		}

		T* newAction = new T();
		newAction->Init();
		newAction->SetTag(ActionName);
		newAction->SetTreeName(m_TagName);
		newAction->SetKeepAction(m_RootSequence);
		newAction->SetActionType(BT_ACTION);
		newAction->SetKeepActionType(BT_NONE);

		m_RootSequence->AddChildAction(newAction);
		m_ActionMap.insert(make_pair(ActionName, newAction));
		m_Count++;

		return newAction;
	}
	template<typename T>
	T* AddRootSelectorInAction(const string& ActionName)
	{
		if (m_RootNode == NULLPTR)
		{
			assert(false);
			return NULLPTR;
		}

		if (m_RootSelector == NULLPTR)
		{
			assert(false);
			return NULLPTR;
		}

		T* newAction = new T();
		newAction->Init();
		newAction->SetTag(ActionName);
		newAction->SetTreeName(m_TagName);
		newAction->SetKeepAction(m_RootSelector);
		newAction->SetActionType(BT_ACTION);
		newAction->SetKeepActionType(BT_NONE);
		
		m_RootSelector->AddChildAction(newAction);
		m_ActionMap.insert(make_pair(ActionName, newAction));
		m_Count++;

		return newAction;
	}
	template<typename T>
	T* AddSequenceInAction(const string& SequenceKeyName, const string& ActionName)
	{
		Sequence* getSequence = FindSequence(SequenceKeyName);

		if (getSequence == NULLPTR)
		{
			assert(false);
			return NULLPTR;
		}

		T* newAction = new T();
		newAction->Init();
		newAction->SetTag(ActionName);
		newAction->SetTreeName(m_TagName);
		newAction->SetKeepAction(getSequence);
		newAction->SetActionType(BT_ACTION);
		newAction->SetKeepActionType(BT_SEQUENCE);

		getSequence->AddChildAction(newAction);
		m_ActionMap.insert(make_pair(ActionName, newAction));
		m_Count++;
		
		return newAction;
	}
	template<typename T>
	T* AddSelectorInAction(const string& SelectorKeyName, const string& ActionName)
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULLPTR)
		{
			assert(false);
			return NULLPTR;
		}

		T* newAction = new T();
		newAction->Init();
		newAction->SetTag(ActionName);
		newAction->SetTreeName(m_TagName);
		newAction->SetKeepAction(getSelector);
		newAction->SetActionType(BT_ACTION);
		newAction->SetKeepActionType(BT_SELECTOR);

		getSelector->AddChildAction(newAction);
		m_ActionMap.insert(make_pair(ActionName, newAction));
		m_Count++;

		return newAction;
	}

	Sequence* AddRootSequenceInSequence(const string& NewSequenceKeyName);
	Selector* AddRootSequenceInSelector(const string& NewSelectorKeyName);
	Sequence* AddRootSelectorInSequence(const string& NewSequenceKeyName);
	Selector* AddRootSelectorInSelector(const string& NewSelectorKeyName);

	Selector* AddSequenceInSelector(const string& SequenceKeyName, const string& SelectorKeyName);
	Sequence* AddSequenceInSequence(const string& OldSequenceKey, const string& NewSequenceKey);
	Selector* AddSelectorInSelector(const string& OldSelectorKey, const string& NewSelector);
	Sequence* AddSelectorInSequence(const string& SelectorKeyName, const string& SequenceKeyName);

	void SetSelectorRandomProcess(const string& SelectorKeyName, bool Value);
	void SequenceTickFuncDisable(const string& SequenceKeyName);
	void SequenceTickFuncEnable(const string& SequenceKeyName);
	void SelectorTickFuncDisable(const string& SelectorKeyName);
	void SelectorTickFuncEnable(const string& SelectorKeyName);

	void AddSquenceInDecorator(const string& SequenceKeyName, bool(*pFunc)(float))
	{
		Sequence* getSquence = FindSequence(SequenceKeyName);

		if (getSquence == NULLPTR)
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

		if (getSquence == NULLPTR)
		{
			assert(false);
			false;
		}

		getSquence->AddDecorator(object, pFunc);
	}

	void AddSelectorInDecorator(const string& SelectorKeyName, bool(*pFunc)(float))
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULLPTR)
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

		if (getSelector == NULLPTR)
		{
			assert(false);
			false;
		}

		getSelector->AddDecorator(object, pFunc);
	}

	void AddActionInDecorator(const string& ActionName, bool(*pFunc)(float))
	{
		Action* getAction = FindAction(ActionName);

		if (getAction == NULLPTR)
		{
			assert(false);
			false;
		}

		getAction->AddDecorator(pFunc);
	}

	template<typename T>
	void AddActionInDecorator(const string& ActionName, T* object, bool(T::*pFunc)(float))
	{
		Action* getAction = FindAction(ActionName);

		if (getAction == NULLPTR)
		{
			assert(false);
			false;
		}

		getAction->AddDecorator(object, pFunc);
	}

	void SetSelectorInTickFunc(const string& SelectorKeyName, float CallbackTime, void(*pFunc)(float))
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULLPTR)
		{
			assert(false);
			false;
		}

		getSelector->AddTickFunc(CallbackTime, pFunc);
	}

	template<typename T>
	void SetSelectorInTickFunc(const string& SelectorKeyName, float CallbackTime, T* object, void(T::*pFunc)(float))
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULLPTR)
		{
			assert(false);
			false;
		}

		getSelector->AddTickFunc(CallbackTime, object, pFunc);
	}

	void SetSequenceInTickFunc(const string& SequenceKeyName, float CallbackTime, void(*pFunc)(float))
	{
		Sequence* getSequence = FindSequence(SequenceKeyName);

		if (getSequence == NULLPTR)
		{
			assert(false);
			false;
		}

		getSequence->AddTickFunc(CallbackTime, pFunc);
	}

	template<typename T>
	void SetSequenceInTickFunc(const string& SequenceKeyName, float CallbackTime, T* object, void(T::*pFunc)(float))
	{
		Sequence* getSequence = FindSequence(SequenceKeyName);

		if (getSequence == NULLPTR)
		{
			assert(false);
			false;
		}

		getSequence->AddTickFunc(CallbackTime, object, pFunc);
	}

	CGameObject* GetTargetObject() const { return m_RootNode->GetTargetObject(); }
	void SetObject(CGameObject* Object);
	string GetRootName() const { return m_RootName; }
	string GetRootSequenceName() const { return m_RootSequenceName; }
	string GetRootSelectorName() const { return m_RootSelectorName; }

	int GetCount() const { return m_Count; }

private:
	void Init(BT_ROOT_CHILD_TYPE eStyle = BT_SEQUENCE, CGameObject* Object = NULLPTR);

	Sequence* FindSequence(const string& KeyName);
	Selector* FindSelector(const string& KeyName);
	Action* FindAction(const string& KeyName);

	void AddRootChildSelector();
	void AddRootChildSequence();

private:
	RootNode* m_RootNode;
	Sequence* m_RootSequence;
	Selector* m_RootSelector;

	unordered_map<string, Sequence*> m_SequenceMap;
	unordered_map<string, Selector*> m_SelectorMap;
	unordered_map<string, Action*> m_ActionMap;

	Action* m_CurAction;
	string m_TagName;
	bool m_isOnePath;
	BT_ACTION_TYPE m_Type;

	string m_RootName;
	string m_RootSequenceName;
	string m_RootSelectorName;
	int m_Count;

	CGameObject* m_Object;

private:
	BehaviorTree();

public:
	~BehaviorTree();

	friend class BTManager;
};