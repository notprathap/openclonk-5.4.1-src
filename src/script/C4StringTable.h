/*
 * OpenClonk, http://www.openclonk.org
 *
 * Copyright (c) 2001-2009, RedWolf Design GmbH, http://www.clonk.de/
 * Copyright (c) 2009-2013, The OpenClonk Team and contributors
 *
 * Distributed under the terms of the ISC license; see accompanying file
 * "COPYING" for details.
 *
 * "Clonk" is a registered trademark of Matthes Bender, used with permission.
 * See accompanying file "TRADEMARK" for details.
 *
 * To redistribute this file separately, substitute the full license texts
 * for the above references.
 */
/* string table: holds all strings used by script engine */

#ifndef C4STRINGTABLE_H

#define C4STRINGTABLE_H

class C4StringTable;
class C4Group;

class C4String
{
	int RefCnt;
public:
	unsigned int Hash;
private:
	StdCopyStrBuf Data; // string data

	explicit C4String(StdStrBuf strString);
	C4String();
	void operator=(const char * s);

	friend class C4StringTable;
public:
	~C4String();

	// Add/Remove Reference
	void IncRef() { ++RefCnt; }
	void DecRef() { if (!--RefCnt) delete this; }

	const char * GetCStr() const { return Data.getData(); }
	StdStrBuf GetData() const { return Data.getRef(); }

};

template <class T>
class C4RefCntPointer
{
public:
	C4RefCntPointer(T* p): p(p) { IncRef(); }
	C4RefCntPointer(): p(0) { }
	template <class U> C4RefCntPointer(const C4RefCntPointer<U> & r): p(r.p) { IncRef(); }
#ifdef HAVE_RVALUE_REF
	// Move constructor
	template <class U> C4RefCntPointer(C4RefCntPointer<U> RREF r): p(r.p) { r.p = 0; }
	// Move assignment
	template <class U> C4RefCntPointer& operator = (C4RefCntPointer<U> RREF r)
	{
		if (p != r.p)
		{
			DecRef();
			p = r.p;
			r.p = 0;
		}
		return *this;
	}
#endif
	~C4RefCntPointer() { DecRef(); }
	template <class U> C4RefCntPointer& operator = (U* new_p)
	{
		if (p != new_p)
		{
			DecRef();
			p = new_p;
			IncRef();
		}
		return *this;
	}
	template <class U> C4RefCntPointer& operator = (const C4RefCntPointer<U>& r)
	{
		return *this = r.p;
	}
	T& operator * () { return *p; }
	const T& operator * () const { return *p; }
	T* operator -> () { return p; }
	const T* operator -> () const { return p; }
	operator T * () { return p; }
	operator const T * () const { return p; }
private:
	void IncRef() { if (p) p->IncRef(); }
	void DecRef() { if (p) p->DecRef(); }
	T * p;
};

template<typename T> class C4Set
{
	unsigned int Capacity;
	unsigned int Size;
	T * Table;
	T * GetPlaceFor(T const & e)
	{
		unsigned int h = Hash(e);
		T * p = &Table[h % Capacity];
		while (*p && !Equals(*p, e))
		{
			p = &Table[++h % Capacity];
		}
		return p;
	}
	T * AddInternal(T const & e)
	{
		T * p = GetPlaceFor(e);
		*p = e;
		return p;
	}
#ifdef HAVE_RVALUE_REF
	T * AddInternal(T && e)
	{
		T * p = GetPlaceFor(e);
		*p = std::move(e);
		return p;
	}
#endif
	void MaintainCapacity()
	{
		if (Capacity - Size < Max(2u, Capacity / 4))
		{
			unsigned int OCapacity = Capacity;
			Capacity *= 2;
			T * OTable = Table;
			Table = new T[Capacity];
			Clear();
			for (unsigned int i = 0; i < OCapacity; ++i)
			{
				if (OTable[i])
					AddInternal(std::move(OTable[i]));
			}
			delete [] OTable;
		}
	}
public:
	template<typename H> static unsigned int Hash(const H &);
	template<typename H> static bool Equals(const T &, const H &);
	static bool Equals(const T & a, const T & b) { return a == b; }
	C4Set(): Capacity(2), Size(0), Table(new T[Capacity])
	{
		Clear();
	}
	~C4Set()
	{
		delete[] Table;
	}
	C4Set(const C4Set & b): Capacity(0), Size(0), Table(0)
	{
		*this = b;
	}
	C4Set & operator = (const C4Set & b)
	{
		Capacity = b.Capacity;
		Size = b.Size;
		delete[] Table;
		Table = new T[Capacity];
		for (unsigned int i = 0; i < Capacity; ++i)
			Table[i] = b.Table[i];
		return *this;
	}
	void CompileFunc(StdCompiler *pComp, C4ValueNumbers *);
	void Clear()
	{
		for (unsigned int i = 0; i < Capacity; ++i)
			Table[i] = 0;
	}
	template<typename H> T & Get(H e) const
	{
		unsigned int h = Hash(e);
		T * r = &Table[h % Capacity];
		while (*r && !Equals(*r, e))
		{
			r = &Table[++h % Capacity];
		}
		return *r;
	}
	template<typename H> bool Has(H e) const
	{
		unsigned int h = Hash(e);
		T * r = &Table[h % Capacity];
		while (*r && !Equals(*r, e))
		{
			r = &Table[++h % Capacity];
		}
		return !!*r;
	}
	unsigned int GetSize() const { return Size; }
	T * Add(T const & e)
	{
		MaintainCapacity();
		T * r = AddInternal(e);
		++Size;
		return r;
	}
#ifdef HAVE_RVALUE_REF
	T * Add(T && e)
	{
		MaintainCapacity();
		T * r = AddInternal(std::move(e));
		++Size;
		return r;
	}
#endif
	template<typename H> void Remove(H e)
	{
		unsigned int h = Hash(e);
		T * r = &Table[h % Capacity];
		while (*r && !Equals(*r, e))
		{
			r = &Table[++h % Capacity];
		}
		assert(*r);
		*r = 0;
		--Size;
		// Move entries which might have collided with e
		while (*(r = &Table[++h % Capacity]))
		{
			T m = *r;
			*r = 0;
			AddInternal(std::move(m));
		}
	}
	T const * First() const { return Next(Table - 1); }
	T const * Next(T const * p) const
	{
		while (++p != &Table[Capacity])
		{
			if (*p) return p;
		}
		return 0;
	}
	void Swap(C4Set<T> * S2)
	{
		unsigned int Capacity2 = S2->Capacity;
		unsigned int Size2 = S2->Size;
		T * Table2 = S2->Table;
		S2->Capacity = Capacity;
		S2->Size = Size;
		S2->Table = Table;
		Capacity = Capacity2;
		Size = Size2;
		Table = Table2;
	}
	static bool SortFunc(const T *p1, const T*p2)
	{
		// elements are guarantueed to be non-NULL
		return *p1<*p2;
	}
	std::list<const T *> GetSortedListOfElementPointers() const
	{
		// return a list of pointers to all elements in this set sorted by the standard less-than operation
		// of the elements
		// elements of resulting lists are guarantueed to be non-NULL
		// list remains valid as long as this set is not changed
		std::list<const T *> result;
		for (const T *p = First(); p; p = Next(p)) result.push_back(p);
		result.sort(C4Set<T>::SortFunc);
		return result;
	}
};

template<> template<>
inline unsigned int C4Set<C4String *>::Hash<const C4String *>(const C4String * const & e)
{
	return e->Hash;
}
template<> template<>
inline unsigned int C4Set<C4String *>::Hash<C4String *>(C4String * const & e)
{
	return e->Hash;
}

enum C4PropertyName
{
	// TODO: documentation comments can be removed
	// as soon as all properties are documented

	P_Prototype,
	P_Name,
	P_Priority,
	P_Interval,
	P_CommandTarget,
	P_Time,
	P_Collectible,
	P_Touchable,
	P_ActMap,
	P_Attach,
	P_Visibility,
	P_Parallaxity,
	P_LineColors,
	P_LineAttach,
	P_LineMaxDistance,		// unused?
	P_PictureTransformation,
	P_MeshTransformation,
	P_Procedure,
	P_Speed,
	P_Accel,
	P_Decel,
	P_Directions,
	P_FlipDir,
	P_Length,
	P_Delay,
	P_X,
	P_Y,
	P_Wdt,
	P_Hgt,
	P_OffX,
	P_OffY,
	P_FacetBase,
	P_FacetTopFace,
	P_FacetTargetStretch,
	P_NextAction,
	P_Hold,
	P_Idle,
	P_NoOtherAction,
	P_StartCall,
	P_EndCall,
	P_AbortCall,
	P_PhaseCall,
	P_Sound,
	P_ObjectDisabled,
	P_DigFree,
	P_InLiquidAction,
	P_TurnAction,
	P_Reverse,
	P_Step,
	P_MouseDrag,
	P_MouseDragImage,
	P_Animation,
	P_Action,
	P_BreatheWater,
	P_CorrosionResist,
	P_MaxEnergy,
	P_MaxBreath,
	P_ThrowSpeed,
	P_Mode,					// unused?
	P_CausedBy,				// unused?
	P_Blasted,				// unused?
	P_IncineratingObj,		// unused?
	P_Plane,
	P_Tooltip,
	P_Placement,
	P_BlastIncinerate,
	P_ContactIncinerate,
	P_Global,
	P_Scenario,
	P_JumpSpeed,
	P_Algo,
	P_Layer,
	P_Seed,
	P_Ratio,
	P_FixedOffset,
	P_Op,
	P_R,
	P_Scale,
	P_Amplitude,
	P_Iterations,
	P_Empty,
	P_Open,
	P_Left,
	P_Top,
	P_Right,
	P_Bottom,
	P_Filter,
	P_ForceX,
	P_ForceY,
	P_G,
	P_B,
	P_Alpha,
	P_DampingX,
	P_DampingY,
	P_Size,
	P_Rotation,
	P_BlitMode,
	P_Phase,
	P_Stretch,
	P_CollisionVertex,
	P_OnCollision,
	P_Distance,
	P_Smoke,
// Default Action Procedures
	DFA_WALK,
	DFA_FLIGHT,
	DFA_KNEEL,
	DFA_SCALE,
	DFA_HANGLE,
	DFA_DIG,
	DFA_SWIM,
	DFA_THROW,
	DFA_BRIDGE,
	DFA_PUSH,
	DFA_LIFT,
	DFA_FLOAT,
	DFA_ATTACH,
	DFA_CONNECT,
	DFA_PULL,
	P_LAST
};

// There is only one Stringtable
class C4StringTable
{
public:
	C4StringTable();
	virtual ~C4StringTable();

	C4String *RegString(StdStrBuf String);
	C4String *RegString(const char * s) { return RegString(StdStrBuf(s)); }
	// Find existing C4String
	C4String *FindString(const char *strString);

private:
	C4Set<C4String *> Set;
	friend class C4String;

public:
	// After the set, so these are destroyed with the set still alive
	C4String P[P_LAST];
};

extern C4StringTable Strings;

#endif