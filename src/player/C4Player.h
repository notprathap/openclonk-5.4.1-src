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

/* Player data at runtime */

#ifndef INC_C4Player
#define INC_C4Player

#include "C4MainMenu.h"
#include "C4ObjectInfoList.h"
#include "C4InfoCore.h"
#include "C4ObjectList.h"
#include "C4ObjectPtr.h"
#include "C4PlayerControl.h"

#include <set>

const int32_t C4PVM_Cursor    = 0,
              C4PVM_Target    = 1,
              C4PVM_Scrolling = 2;

const int32_t C4P_Number_None = -5;

const int32_t C4MaxPlayer = 5000; // ought to be enough for everybody (used to catch invalid player counts)
const int32_t C4MaxClient = 5000; // ought to be enough for everybody (used to catch invalid client counts)

// view ranges in "CR-pixels" covered by viewport
static const int C4VP_DefViewRangeX    = 300,
                 C4VP_DefMinViewRangeX = 150,
                 C4VP_DefMaxViewRangeX = 750;
#define C4FOW_Def_View_RangeX 500

class C4Player: public C4PlayerInfoCore
{
	class HostilitySet : public std::set<const C4Player*>
	{
	public:
		void CompileFunc(StdCompiler *pComp);
	};

public:
	// possible player controls used for statistics
	enum ControlType
	{
		PCID_None,      // no control
		PCID_Message,   // chat
		PCID_Special,   // goalruleinfo, etc.
		PCID_Command,   // mouse control
		PCID_DirectCom  // menu or keyboard control
	};
	// possible status
	enum StatusTypes
	{
		PS_None=0,            // player disabled
		PS_Normal=1,          // normal playing
		PS_TeamSelection = 2, // team selection
		PS_TeamSelectionPending = 3 // waiting for team selection packet to come through
	};
	friend class C4PlayerList;

public:
	C4Player();
	~C4Player();
public:
	char Filename[_MAX_PATH+1];
	StdStrBuf Name;
	int32_t Status;
	int32_t Eliminated;
	int32_t Surrendered;
	bool Evaluated;
	int32_t Number;
	int32_t ID; // unique player ID
	int32_t Team; // team ID - may be 0 for no teams
	uint32_t ColorDw;
	class C4PlayerControlAssignmentSet *ControlSet;
	StdCopyStrBuf ControlSetName;
	int32_t MouseControl;
	int32_t Position;
	int32_t PlrStartIndex;
	int32_t RetireDelay;
	int32_t GameJoinTime; // Local-NoSave - set in Init; reset in LocalSync
	int32_t AtClient;
	char AtClientName[C4MaxTitle+1];
	bool LocalControl; // Local-NoSave //
	bool LeagueEvaluated; // Local-NoSave //
	bool NoEliminationCheck; // Local-NoSave. Always set in init and restore by info //
	// Menu
	C4MainMenu Menu; // Local-NoSave //
	// View
	int32_t ViewMode;
	C4Real ViewX,ViewY; // FIXED because this is saved/synchronized
	bool ViewLock; // if set, no scrolling is allowed
	C4Object *ViewTarget; // NoSave //
	bool ShowStartup;
	int32_t FlashCom; // NoSave //
	bool fFogOfWar;
	bool fFogOfWarInitialized; // No Save //
	C4ObjectList FoWViewObjs; // No Save //
	int32_t ZoomLimitMinWdt,ZoomLimitMinHgt,ZoomLimitMaxWdt,ZoomLimitMaxHgt,ZoomWdt,ZoomHgt; // zoom limits and last zoom set by script
	// Game
	int32_t Wealth;
	int32_t CurrentScore,InitialScore;
	int32_t ObjectsOwned;
	HostilitySet Hostility;
	// Home Base
	C4IDList HomeBaseMaterial;
	C4IDList HomeBaseProduction;
	int32_t ProductionDelay,ProductionUnit;
	// Crew
	C4ObjectInfoList CrewInfoList; // No Save //
	C4ObjectList Crew; // Save new in 4.95.2 (for sync reasons)
	int32_t CrewCnt; // No Save //
	// Knowledge
	C4IDList Knowledge;
	// Control
	C4PlayerControl Control;
	C4ObjectPtr Cursor, ViewCursor;
	int32_t CursorFlash;
	class C4GamePadOpener *pGamepad;
	// Message
	int32_t MessageStatus;
	char MessageBuf[256+1];
	class C4MessageBoardQuery *pMsgBoardQuery;
	// BigIcon
	C4FacetSurface BigIcon;
	// Dynamic list
	C4Player *Next;

	// statistics
	class C4TableGraph *pstatControls, *pstatActions;
	int32_t ControlCount; // controls issued since value was last recorded
	int32_t ActionCount;  // non-doubled controls since value was last recorded
	ControlType LastControlType; int32_t LastControlID; // last control to capture perma-pressers in stats

public:
	const char *GetName() const { return Name.getData(); }
	C4PlayerType GetType() const;

public:
	void Eliminate();
	void Default();
	void Clear();
	void ClearPointers(C4Object *tptr, bool fDeath);
	void Execute();
	void ExecuteControl();
	void SetViewMode(int32_t iMode, C4Object *pTarget=NULL);
	void ResetCursorView(); // reset view to cursor if any cursor exists
	void Evaluate();
	void Surrender();
	void ScrollView(float iX, float iY, float ViewWdt, float ViewHgt); // in landscape coordinates
	void SetCursor(C4Object *pObj, bool fSelectArrow);
	void RemoveCrewObjects();
	int32_t FindNewOwner() const;
	void NotifyOwnedObjects();
	void DefaultRuntimeData();
	void DrawHostility(C4Facet &cgo, int32_t iIndex);
	void AdjustCursorCommand();
	void CursorRight();
	void CursorLeft();

	bool ObjectCommand(int32_t iCommand, C4Object *pTarget, int32_t iTx, int32_t iTy, C4Object *pTarget2=NULL, C4Value iData=C4VNull, int32_t iAddMode=C4P_Command_Set);
	void ObjectCommand2Obj(C4Object *cObj, int32_t iCommand, C4Object *pTarget, int32_t iX, int32_t iY, C4Object *pTarget2, C4Value iData, int32_t iMode);
	bool DoScore(int32_t iChange);
	bool Init(int32_t iNumber, int32_t iAtClient, const char *szAtClientName, const char *szFilename, bool fScenarioInit, class C4PlayerInfo *pInfo, C4ValueNumbers *);
	bool ScenarioAndTeamInit(int32_t idTeam);
	bool ScenarioInit();
	bool FinalInit(bool fInitialScore);
	bool Save();
	bool Save(C4Group &hGroup, bool fSavegame, bool fStoreTiny);
	bool MakeCrewMember(C4Object *pObj, bool fForceInfo=true, bool fDoCalls=true);
	bool Load(const char *szFilename, bool fSavegame);
	static bool Strip(const char *szFilename, bool fAggressive);
	bool Message(const char *szMsg);
	bool ObjectInCrew(C4Object *tobj);
	bool DoWealth(int32_t change);
	bool SetWealth(int32_t val);
	bool SetHostility(int32_t iOpponent, int32_t iHostility, bool fSilent=false);
	bool IsHostileTowards(const C4Player *opponent) const;
	void CompileFunc(StdCompiler *pComp, C4ValueNumbers *);
	void DenumeratePointers();
	bool LoadRuntimeData(C4Group &hGroup, C4ValueNumbers *);
	bool ActivateMenuMain();
	bool ActivateMenuTeamSelection(bool fFromMain);
	void DoTeamSelection(int32_t idTeam);
	C4Object *GetHiExpActiveCrew();
	C4Object *GetHiRankActiveCrew();
	void SetFoW(bool fEnable);
	int32_t ActiveCrewCount();
	int32_t GetSelectedCrewCount();
	bool LocalSync(); // sync InAction et. al. back o local player file
	bool SetObjectCrewStatus(C4Object *pCrew, bool fNewStatus); // add/remove object from crew
	bool IsChosingTeam() const { return Status==PS_TeamSelection || Status==PS_TeamSelectionPending; }
	bool IsInvisible() const;
	bool IsViewLocked() const { return ViewLock; } // return if view is fixed to cursor, so scrolling is not allowed
	void SetViewLocked(bool to_val); // lock or unlock free scrolling for player

protected:
	void ClearControl();
	void InitControl();
	void UpdateView();
	void CheckElimination();
	void UpdateCounts();
	void ExecHomeBaseProduction();
	void PlaceReadyBase(int32_t &tx, int32_t &ty, C4Object **pFirstBase);
	void PlaceReadyVehic(int32_t tx1, int32_t tx2, int32_t ty, C4Object *FirstBase);
	void PlaceReadyMaterial(int32_t tx1, int32_t tx2, int32_t ty, C4Object *FirstBase);
	void PlaceReadyCrew(int32_t tx1, int32_t tx2, int32_t ty, C4Object *FirstBase);
	void CheckCrewExPromotion();

public:
	void SetTeamHostility(); // if Team!=0: Set hostile to all players in other teams and allied to all others (both ways)

	void CloseMenu(); // close all player menus (keep sync object menus!)

	void EvaluateLeague(bool fDisconnected, bool fWon);

	void FoW2Map(C4FogOfWar &rMap, int iOffX, int iOffY);
	void FoWGenerators2Map(C4FogOfWar &rMap, int iOffX, int iOffY);
	bool FoWIsVisible(int32_t x, int32_t y); // check whether a point in the landscape is visible

	// runtime statistics
	void CreateGraphs();
	void ClearGraphs();
	void CountControl(ControlType eType, int32_t iID, int32_t iCntAdd=1);

	class C4PlayerInfo *GetInfo(); // search info by ID

private:
	// messageboard-calls for this player
	class C4MessageBoardQuery *GetMessageboardQuery(C4Object *pForObj);
	void ExecMsgBoardQueries();
public:
	void ToggleMouseControl();
	void CallMessageBoard(C4Object *pForObj, const StdStrBuf &sQueryString, bool fUppercase);
	bool RemoveMessageBoardQuery(C4Object *pForObj);
	bool MarkMessageBoardQueryAnswered(C4Object *pForObj);
	bool HasMessageBoardQuery(); // return whether any object has a messageboard-query

	// callback by script execution of team selection: Restart team selection if the team turned out to be not available
	void OnTeamSelectionFailed();

	// when the player changes team, his color changes. Relfect this in player objects
	void SetPlayerColor(uint32_t dwNewClr);

	// zoom and zoom limit changes
	void SetZoomByViewRange(int32_t range_wdt, int32_t range_hgt, bool direct, bool no_increase, bool no_decrease);
	void SetMinZoomByViewRange(int32_t range_wdt, int32_t range_hgt, bool no_increase, bool no_decrease);
	void SetMaxZoomByViewRange(int32_t range_wdt, int32_t range_hgt, bool no_increase, bool no_decrease);
	void ZoomToViewports(bool direct, bool no_increase=false, bool no_decrease=false);
	void ZoomToViewport(C4Viewport* vp, bool direct, bool no_increase=false, bool no_decrease=false);
	void ZoomLimitsToViewports();
	void ZoomLimitsToViewport(C4Viewport* vp);

private:
	bool AdjustZoomParameter(int32_t *range_par, int32_t new_val, bool no_increase, bool no_decrease);
};

#endif
