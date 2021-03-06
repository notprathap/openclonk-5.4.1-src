/**
	Expansion
	A certain amount of the map must be claimed by constructing flags.
	This is tested by a Monte Carlo simulation over the whole map,
	checking whether the point is covered by a flagpole. 
	
	@author Maikel	
*/

#include Library_Goal


local expansion_goal;
local mc_data;

protected func Initialize()
{
	expansion_goal = 0;
	// Start running the monte carlo simulation.
	mc_data = [];
	AddEffect("IntAreaMonteCarlo", this, 100, 1, this);
	return inherited(...);
}

// Set the expansion goal in promillage of the total map.
public func SetExpansionGoal(int exp_area)
{
	expansion_goal = exp_area;
	return;
}

// Returns the expansion goal in promillage of the total map.
public func GetExpansionGoal()
{
	return expansion_goal;
}

// Scenario saving
public func SaveScenarioObject(props)
{
	if (!inherited(props, ...)) return false;
	if (expansion_goal) props->AddCall("Goal", this, "SetExpansionGoal", expansion_goal);
	return true;
}

/*-- Goal interface --*/

// The goal is fulfilled if the expansio goal is covered by flags.
public func IsFulfilled()
{
	// Check if goal is fulfilled.
	if (GetExpansionArea() >= GetExpansionGoal())
		return true;
	// Not yet fulfilled.
	return false;
}

// Returns the promillage of the total map covered by flags.
private func GetExpansionArea()
{
	// Evaluate mc simulation.
	var area = 0, cnt = 0;
	for (var mc in mc_data)
	{
		if (mc != nil)
		{
			area += mc;
			cnt++;
		}	
	}
	if (!cnt)
		return 0;
	return area / cnt;
}

// Monte Carlo simulation to determine the area covered by flagpoles.
// There is thin balance between the accuracy of the simulation, the 
// number of simulations per frame and the refresh rate of the data,
// or equivalently how long past data is stored. 
private func FxIntAreaMonteCarloTimer()
{
	// Perform simulations and store them.
	var cnt = 40; // Perform 40 simulations per frame.
	var store = 500; // Store last 500 events.
	var rate = 0;
	for (var i = 0; i < cnt; i++)
	{
		var x = Random(LandscapeWidth());
		var y = Random(LandscapeHeight());
		if (CoveredByFlag(x, y))
			rate++;	
	}
	var promille = 1000 * rate / cnt;
	// Add new montecarlo data to the end the data list and remove old entry.
	for (var i = 0; i < store - 1; i++)
	{
		mc_data[i] = mc_data[i+1];	
	}
	mc_data[store - 1] = promille;
	return;
}

// Returns whether the point (x,y) is covered by a flagpole.
private func CoveredByFlag(int x, int y)
{
	for (var flag in FindObjects(Find_Func("IsFlagpole"), Sort_Distance(x - GetX(), y - GetY())))
		if (Distance(flag->GetX(), flag->GetY(), x, y) < flag->GetFlagRadius())
			return true;
	return false;
}

// Shows or hides a message window with information.
public func Activate(int plr)
{
	// If goal message open -> hide it.
	if (GetEffect("GoalMessage", this))
	{
		CustomMessage("", nil, plr, nil, nil, nil, nil, nil, MSG_HCenter);
		RemoveEffect("GoalMessage", this);
		return;
	}
	// Otherwise open a new message.
	AddEffect("GoalMessage", this, 100, 0, this);
	var message;
	if (GetExpansionArea() >= GetExpansionGoal())
		message = Format("@$MsgGoalFulfilled$");	
	else
		message = Format("@$MsgGoalUnFulfilled$", 100 * GetExpansionArea() / GetExpansionGoal());

	CustomMessage(message, nil, plr, 0, 16 + 64, 0xffffff, GUI_MenuDeco, this, MSG_HCenter);
	return;
}

protected func FxGoalMessageStart() {}

public func GetShortDescription(int plr)
{
	// Show expansion percentage.
	var perc = Min(100, 100 * GetExpansionArea() / GetExpansionGoal());
	var clr = RGB(255, 0, 0);
	if (perc >= 100)
		clr = RGB(0, 255, 0);
	var msg = Format("<c %x>%d%</c>", clr, perc);
	return msg;
}

/*-- Proplist --*/

local Name = "$Name$";
