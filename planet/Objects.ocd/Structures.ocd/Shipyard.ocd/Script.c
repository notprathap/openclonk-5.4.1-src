/*-- Shipyard --*/

#include Library_Structure
#include Library_Ownable
#include Library_Producer

local animWork;
local meshAttach;

func Initialize()
{
	animWork = PlayAnimation("Work", 1, Anim_Const(0), Anim_Const(1000));
	return _inherited(...);
}

func Construction(object creator)
{
	SetAction("Wait");
	return _inherited(creator, ...);
}

/*-- Production --*/

public func IsProduct(id product_id)
{
	return product_id->~IsShipyardProduct();
}

private func ProductionTime(id toProduce) { return 400; }
private func PowerNeed() { return 150; }

public func NeedRawMaterial(id rawmat_id)
{
	return true;
}

private func FxIntWorkAnimTimer(object target, proplist effect, int timer)
{
	if(effect.paused == true) return 1;
	
//	Message("Working...");

	var tickAmount = 50;
	var animSpot = GetAnimationPosition(animWork);
	//-50 is to dodge around an engine crash. If it reaches (near) the end of the
	//animation, it dies for some reason. :(
	var animLength = GetAnimationLength("Work") - 50;
	
	//loop anim
	if(animSpot + tickAmount > animLength){
		SetAnimationPosition(animWork, Anim_Const(animSpot + tickAmount - animLength));
	}
	//otherwise, advance animation
	else SetAnimationPosition(animWork, Anim_Const(animSpot + tickAmount));
}

local workEffect;

public func OnProductionStart(id product)
{
	workEffect = AddEffect("IntWorkAnim", this, 1,1,this);
	return _inherited(...);
}

public func OnProductionHold(id product)
{
	workEffect.paused = true;
	return _inherited(...);
}

public func OnProductionContinued(id product)
{
	workEffect.paused = false;
	return _inherited(...);
}

public func OnProductionFinish(id product)
{
	RemoveEffect(nil, this, workEffect);
	return _inherited(...);
}

func Definition(def){
	SetProperty("MeshTransformation", Trans_Rotate(8, 0,1,0), def);
	SetProperty("PictureTransformation", Trans_Mul(Trans_Translate(0,-25000,50000), Trans_Scale(500)), def);
}

local ActMap = {
	Wait = {
		Prototype = Action,
		Name = "Wait",
		Procedure = DFA_NONE,
		Directions = 2,
		FlipDir = 1,
		Length = 1,
		Delay = 0,
		FacetBase=1,
		NextAction = "Wait",
	},
};

local Name = "$Name$";
local Description ="$Description$";
local BlastIncinerate = 100;
local HitPoints = 70;
