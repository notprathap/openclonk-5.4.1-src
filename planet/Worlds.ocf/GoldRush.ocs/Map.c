/**
	Gold Rush
	Dynamic map a few layers of materials below a flat shaped earth surface.
	
	@authors Maikel
*/

#include Library_Map


// Scenario properties which can be set later by the lobby options.
static const SCENOPT_MapSize = 1;

// Called be the engine: draw the complete map here.
protected func InitializeMap(proplist map)
{
	// Retrieve the settings according to the MapSize setting.
	var map_size;
	if (SCENOPT_MapSize == 1)
		map_size = [100, 75]; 
	if (SCENOPT_MapSize == 2)
		map_size = [125, 90];
	if (SCENOPT_MapSize == 3)
		map_size = [150, 105];
	
	// Set the map size.
	map->Resize(map_size[0], map_size[1]);
	
	// Draw the main surface: a rectangle with some turbulence on top makes.
	var rect = {X = 0, Y = 4 * map.Hgt / 10, Wdt = map.Wdt,  Hgt = 6 * map.Hgt / 10};
	var surface = {Algo = MAPALGO_Rect, X = rect.X, Y = rect.Y, Wdt = rect.Wdt, Hgt = 8 * rect.Hgt / 6};
	surface = {Algo = MAPALGO_Turbulence, Iterations = 4, Amplitude = [0, 12], Seed = Random(65536), Op = surface};
	Draw("Earth", surface);	
	
	// Draw materials inside the main surface.
	DrawMaterials(rect, surface);
	
	// Return true to tell the engine a map has been successfully created.
	return true;
}

// Draws materials on the given surface.
public func DrawMaterials(proplist rect, proplist surface)
{
	var mask;
	var x = rect.X;
	var y = rect.Y;
	var wdt = rect.Wdt;
	var hgt = rect.Hgt;
	
	// A bit of different types of earth all around the surface.
	mask = {Algo = MAPALGO_Rect, X = x,  Y = y, Wdt = wdt, Hgt = hgt};
	mask = {Algo = MAPALGO_And, Op = [surface, mask]};
	DrawMaterial("Earth-earth_topsoil", mask, 4, 12);
	DrawMaterial("Earth-earth_rough", mask, 2, 16);
	DrawMaterial("Earth-earth_dry", mask, 2, 16);
	DrawMaterial("Earth-earth_midsoil", mask, 4, 12);

	// Coal and surface in the first layer.
	mask = {Algo = MAPALGO_Rect, X = x,  Y = y, Wdt = wdt, Hgt = hgt / 4};
	mask = {Algo = MAPALGO_Turbulence, Iterations = 4, Op = mask};
	mask = {Algo = MAPALGO_And, Op = [surface, mask]}; 
	DrawMaterial("Firestone", mask, 4, 5);
	DrawMaterial("Coal", mask, 4, 5);
	DrawMaterial("Firestone", mask);
	DrawMaterial("Coal", mask);
	
	// Some small lakes as well in a second layer .
	mask = {Algo = MAPALGO_Rect, X = x,  Y = y + 1 * hgt / 4, Wdt = wdt, Hgt = hgt / 4};
	mask = {Algo = MAPALGO_Turbulence, Iterations = 4, Op = mask};
	mask = {Algo = MAPALGO_And, Op = [surface, mask]};
	DrawMaterial("Coal", mask, 3, 8);
	DrawMaterial("Firestone", mask, 4, 5);
	DrawMaterial("Water", mask, 4, 10);
	
	// Ore and rock in the third layer.
	mask = {Algo = MAPALGO_Rect, X = x,  Y = y + 2 * hgt / 4, Wdt = wdt, Hgt = hgt / 4};
	mask = {Algo = MAPALGO_Turbulence, Iterations = 4, Op = mask};
	mask = {Algo = MAPALGO_And, Op = [surface, mask]}; 
	DrawMaterial("Ore", mask, 3, 10);	
	DrawMaterial("Rock-rock_cracked", mask, 2, 8);
	DrawMaterial("Granite", mask, 2, 8);
	DrawMaterial("Rock", mask);
	DrawMaterial("Ore", mask);	
	
	// Gold in the last layer.
	mask = {Algo = MAPALGO_Rect, X = x,  Y = y + 3 * hgt / 4, Wdt = wdt, Hgt = hgt / 4};
	mask = {Algo = MAPALGO_Turbulence, Iterations = 4, Op = mask};
	mask = {Algo = MAPALGO_And, Op = [surface, mask]}; 
	DrawMaterial("Gold", mask, 2, 5);
	DrawMaterial("Coal", mask, 2, 10);
	DrawMaterial("Gold", mask, 2, 5);
	DrawMaterial("Gold", mask, 5, 10);

	// The top border consists of top soil and dry earth and a bit of sand.
	var border = {Algo = MAPALGO_Border, Top = 4, Op = surface};
	Draw("Earth", border);
	var rnd_checker = {Algo = MAPALGO_RndChecker, Ratio = 30, Wdt = 2, Hgt = 2};
	var rnd_border = {Algo = MAPALGO_And, Op = [border, rnd_checker]};
	Draw("Sand", rnd_border);
	Draw("Earth-earth_topsoil", rnd_border);

	return;
} 

// Draws some material inside an island.
public func DrawMaterial(string mat, proplist onto_mask, int speck_size, int ratio)
{
	if (!speck_size)
		speck_size = 4;
	if (!ratio)
		ratio = 15;
	// Use random checker algorithm to draw patches of the material. 
	var rnd_checker = {Algo = MAPALGO_RndChecker, Ratio = ratio, Wdt = speck_size, Hgt = speck_size};
	rnd_checker = {Algo = MAPALGO_Turbulence, Iterations = 4, Op = rnd_checker};
	var algo = {Algo = MAPALGO_And, Op = [onto_mask, rnd_checker]};
	Draw(mat, algo);
	
	return;
}
