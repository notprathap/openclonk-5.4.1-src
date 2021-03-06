/**
	Team account
	Allied players share a common account.
	
	@author Maikel
*/


protected func Initialize()
{
	// Under no circumstance there may by multiple copies of this rule.
	if (ObjectCount(Find_ID(Rule_TeamAccount)) > 1)
		return RemoveObject();
	return;
}

// Only SetWealth needs to be overloaded, DoWealth just uses that.
global func SetWealth(int plr, int wealth)
{
	// Only if team account rule is activated.
	if (!FindObject(Find_ID(Rule_TeamAccount)))
		return _inherited(plr, wealth, ...);
		
	// Only for valid players.
	if (plr == NO_OWNER || !GetPlayerName(plr))
		return _inherited(plr, wealth, ...);
		
	// Also set wealth of all allies.
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var to_plr = GetPlayerByIndex(i);
		if (to_plr != plr && !Hostile(to_plr, plr))
			_inherited(to_plr, wealth, ...);
	}

	return _inherited(plr, wealth, ...);
}

protected func InitializePlayer(int plr)
{
	// Find an ally and add this wealth.
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var to_plr = GetPlayerByIndex(i);
		if (to_plr != plr && !Hostile(to_plr, plr))
		{
			DoWealth(to_plr, GetWealth(plr));
			break;
		}
	}	
	return;
}

protected func RemovePlayer(int plr)
{
	// Nothing should happen here.
	return;
}

protected func OnTeamSwitch(int player, int new_team, int old_team)
{
	// Remove player from old team, i.e. substract his fair share.
	var count = 0;
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		if (!Hostile(player, GetPlayerByIndex(i)))
			count++;		
	}
	//var share = GetWealth(player) / count;

	// Add player to new team, i.e. add his wealth.
	// TODO Implement
	return;
}

protected func OnHostilityChange(int plr, int plr2, bool hostility, bool old_hostility)
{
	// TODO: Implement
	return;
}

protected func Activate(int byplr)
{
	MessageWindow("$Description$", byplr);
	return;
}

/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
