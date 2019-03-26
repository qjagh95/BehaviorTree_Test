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
		vector<function<bool(float)>>* GetDecoratorVec() { return &m_vecDecorator; }

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
		Action() { m_KeepNode = NULL; m_pObject = NULL; m_Type = ACTION_NONE; }

		vector<function<bool(float)>> m_vecDecorator;
		string m_TagName;
		string m_TreeName;
		BT_ACTION_TYPE m_Type;
		Action* m_KeepNode;

	private:
		CGameObject* m_pObject;
	};

	class CompositNode : public Action
	{
	public:
		void AddChild(Action* Node) { m_ChildList.push_back(Node); }
		vector<Action*>* GetChildList() { return &m_ChildList; }

	protected:
		void SetAllActionObject(CGameObject* Object);

	protected:
		vector<Action*> m_ChildList;

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

	private:
		void Render();
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

	private:
		void Render();

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
		void Render();

	private:
		Action* m_ChildNode;

	private:
		~RootNode() {}

	public:
		friend class BehaviorTree;
	};

public:
	void Update(float DeltaTime);

	void AddRootSequenceInSequence(const string& NewSequenceKeyName);
	void AddRootSequenceInSelector(const string& NewSelectorKeyName);
	void AddRootSequenceInAction(const string& SequenceKeyName, const string& ActionName, Action* NewAction);
	void AddRootSelectorInSequence(const string& NewSequenceKeyName);
	void AddRootSelectorInSelector(const string& NewSelectorKeyName);
	void AddRootSelectorInAction(const string& SelectorKeyName, const string& ActionName, Action* NewAction);
	void AddSequenceInAction(const string& SequenceKeyName, const string& ActionName, Action* NewAction);
	void AddSelectorInAction(const string& SelectorKeyName, const string& ActionName, Action* NewAction);
	void AddSequenceInSelector(const string& SequenceKeyName, const string& SelectorKeyName);
	void AddSequenceInSequence(const string& OldSequenceKey, const string& NewSequenceKey);
	void AddSelectorInSelector(const string& OldSelectorKey, const string& NewSelector);
	void AddSelectorInSequence(const string& SelectorKeyName, const string& SequenceKeyName);
	void SetSelectorRandomProcess(const string& SelectorKeyName, bool Value);
	void SequenceTickFuncDisable(const string& SequenceKeyName);
	void SequenceTickFuncEnable(const string& SequenceKeyName);
	void SelectorTickFuncDisable(const string& SelectorKeyName);
	void SelectorTickFuncEnable(const string& SelectorKeyName);

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

	void SetSelectorInTickFunc(const string& SelectorKeyName, float CallbackTime, void(*pFunc)(float))
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
	void SetSelectorInTickFunc(const string& SelectorKeyName, float CallbackTime, T* object, void(T::*pFunc)(float))
	{
		Selector* getSelector = FindSelector(SelectorKeyName);

		if (getSelector == NULL)
		{
			assert(false);
			false;
		}

		getSelector->AddTickFunc(CallbackTime, object, pFunc);
	}

	void AddActionInDecorator(const string& ActionName, bool(*pFunc)(float))
	{
		Action* getAction = FindAction(ActionName);

		if (getAction == NULL)
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

		if (getAction == NULL)
		{
			assert(false);
			false;
		}

		getAction->AddDecorator(object, pFunc);
	}

	void SetSequenceInTickFunc(const string& SequenceKeyName, float CallbackTime, void(*pFunc)(float))
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
	void SetSequenceInTickFunc(const string& SequenceKeyName, float CallbackTime, T* object, void(T::*pFunc)(float))
	{
		Sequence* getSequence = FindSequence(SequenceKeyName);

		if (getSequence == NULL)
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
	void Init(BT_ROOT_CHILD_TYPE eStyle = BT_SEQUENCE);
	void GUIRender();

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

private:
	BehaviorTree();

public:
	~BehaviorTree();

	friend class BTManager;
};