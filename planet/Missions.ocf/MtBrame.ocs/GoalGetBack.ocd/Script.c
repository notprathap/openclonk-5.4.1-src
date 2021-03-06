/*--
		Get back goal
		Author: ck
		
		All players have to get back to the hut
--*/


#include Library_Goal

protected func Initialize()
{
	return inherited(...);
}

public func IsFulfilled()
{
	var cabin = FindObject(Find_ID(WoodenCabin));
	if (!cabin)
		return false;
	if (!GetPlayerCount() || GetEffect("IntIntro"))
		return false;

	for(var i = 0; i < GetPlayerCount(); ++i)
	{
		var plr = GetPlayerByIndex(i), obj;
		for(var j = 0; obj = GetCrew(plr, j); ++j)
			if(ObjectDistance(obj, cabin) > 80)
				return false;
	}

	return true;
}

public func Activate(int byplr)
{
	if (IsFulfilled())
		MessageWindow("$MsgGoalFulfilled$", byplr);
	else
		MessageWindow("$MsgGoalUnFulfilled$", byplr);
	return;
}

public func GetShortDescription(int plr)
{
	return "{{WoodenCabin}}"; // TODO
}

/*-- Proplist --*/
local Name = "$Name$";
