/*
 * OpenClonk, http://www.openclonk.org
 *
 * Copyright (c) 1998-2000, Matthes Bender
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

/* Object definition */

#ifndef INC_C4DefList
#define INC_C4DefList

#include <C4FontLoader.h>

class C4DefList: public CStdFont::CustomImages
{
public:
	C4DefList();
	virtual ~C4DefList();
public:
	bool LoadFailure;
	typedef std::map<C4ID, C4Def*> Table;
	Table table;
protected:
	C4Def *FirstDef;
public:
	void Default();
	void Clear();
	int32_t Load(C4Group &hGroup,
	             DWORD dwLoadWhat, const char *szLanguage,
	             C4SoundSystem *pSoundSystem = NULL,
	             bool fOverload = false,
	             bool fSearchMessage = false, int32_t iMinProgress=0, int32_t iMaxProgress=0, bool fLoadSysGroups = true);
	int32_t Load(const char *szFilename,
	             DWORD dwLoadWhat, const char *szLanguage,
	             C4SoundSystem *pSoundSystem = NULL,
	             bool fOverload = false, int32_t iMinProgress=0, int32_t iMaxProgress=0);
	C4Def *ID2Def(C4ID id);
	C4Def *GetDef(int32_t Index);
	C4Def *GetByPath(const char *szPath);
	int32_t GetDefCount();
	int32_t GetIndex(C4ID id);
	int32_t RemoveTemporary();
	int32_t CheckEngineVersion(int32_t ver1, int32_t ver2, int32_t ver3);
	int32_t CheckRequireDef();
	void Draw(C4ID id, C4Facet &cgo, bool fSelected, int32_t iColor);
	void Remove(C4Def *def);
	bool Remove(C4ID id);
	bool Reload(C4Def *pDef, DWORD dwLoadWhat, const char *szLanguage, C4SoundSystem *pSoundSystem = NULL);
	bool Add(C4Def *ndef, bool fOverload);
	void BuildTable();
	void ResetIncludeDependencies(); // resets all pointers into foreign definitions caused by include chains
	void CallEveryDefinition();
	void Synchronize();

	// callback from font renderer: get ID image
	virtual bool DrawFontImage(const char* szImageTag, C4Facet& rTarget, C4DrawTransform* pTransform);
	virtual float GetFontImageAspect(const char* szImageTag);
};

extern C4DefList Definitions;

inline C4Def *C4Id2Def(C4ID id)
{
	return ::Definitions.ID2Def(id);
}

#endif

