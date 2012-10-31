#include "simulation/Elements.h"
//#TPT-Directive ElementClass Element_VIBR PT_VIBR 165
Element_VIBR::Element_VIBR()
{
	Identifier = "DEFAULT_PT_VIBR";
	Name = "VIBR";
	Colour = PIXPACK(0x005000);
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.85f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 100;

	Temperature = 273.15f;
	HeatConduct = 251;
	Description = "Vibranium. Stores energy and releases it in violent explosions.";

	State = ST_SOLID;
	Properties = TYPE_SOLID|PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_VIBR::update;
	Graphics = &Element_VIBR::graphics;
}

void transferProp(UPDATE_FUNC_ARGS, int propOffset)
{
	int r, rx, ry, trade, transfer;
	for (trade = 0; trade < 9; trade++)
	{
		int random = rand();
		rx = random%7-3;
		ry = (random>>3)%7-3;
		if (x+rx>=0 && y+ry>0 && x+rx<XRES && y+ry<YRES && (rx || ry))
		{
			r = pmap[y+ry][x+rx];
			if ((r&0xFF)!=PT_VIBR)
				continue;
			if (*((int*)(((char*)&parts[i])+propOffset)) > *((int*)(((char*)&parts[r>>8])+propOffset)))
			{
				transfer = *((int*)(((char*)&parts[i])+propOffset)) - *((int*)(((char*)&parts[r>>8])+propOffset));
				if (transfer == 1)
				{
					*((int*)(((char*)&parts[r>>8])+propOffset)) += 1;
					*((int*)(((char*)&parts[i])+propOffset)) -= 1;
					trade = 9;
				}
				else if (transfer > 0)
				{
					*((int*)(((char*)&parts[r>>8])+propOffset)) += transfer/2;
					*((int*)(((char*)&parts[i])+propOffset)) -= transfer/2;
					trade = 9;
				}
			}
		}
	}	
}

//#TPT-Directive ElementHeader Element_VIBR static int update(UPDATE_FUNC_ARGS)
int Element_VIBR::update(UPDATE_FUNC_ARGS) {
	int r, rx, ry;
	if (parts[i].ctype == 1)
	{
		if (sim->pv[y/CELL][x/CELL] > -2.5 || parts[i].tmp) //newly created BVBR turns to VIBR
		{
			parts[i].ctype = 0;
			sim->part_change_type(i, x, y, PT_VIBR);
		}
	}
	else if (!parts[i].life)
	{
		//Heat absorption code
		if (parts[i].temp > 274.65f)
		{
			parts[i].tmp++;
			parts[i].temp -= 3;
		}
		if (parts[i].temp < 271.65f)
		{
			parts[i].tmp--;
			parts[i].temp += 3;
		}
		//Pressure absorption code
		if (sim->pv[y/CELL][x/CELL] > 2.5)
		{
			parts[i].tmp += 10;
			sim->pv[y/CELL][x/CELL]--;
		}
		if (sim->pv[y/CELL][x/CELL] < -2.5)
		{
			parts[i].tmp -= 2;
			sim->pv[y/CELL][x/CELL]++;
		}
	}
	//Release sparks before explode
	if (parts[i].life && parts[i].life < 300)
	{
		rx = rand()%3-1;
		ry = rand()%3-1;
		r = pmap[y+ry][x+rx];
		if ((r&0xFF) && (r&0xFF) != PT_BREC && (sim->elements[r&0xFF].Properties&PROP_CONDUCTS) && !parts[r>>8].life)
		{
			parts[r>>8].life = 4;
			parts[r>>8].ctype = r>>8;
			sim->part_change_type(r>>8,x+rx,y+ry,PT_SPRK);
		}
	}
	//initiate explosion counter
	if (!parts[i].life && parts[i].tmp > 1000)
		parts[i].life = 750;
	//Release all heat
	if (parts[i].life && parts[i].life < 500)
	{
		int random = rand();
		rx = random%7-3;
		ry = (random>>3)%7-3;
		if(x+rx>=0 && y+ry>0 && x+rx<XRES && y+ry<YRES)
		{
			r = pmap[y+ry][x+rx];
			if ((r&0xFF) && (r&0xFF)!=PT_VIBR)
			{
				parts[r>>8].temp += parts[i].tmp*6;
				parts[i].tmp -= parts[i].tmp*2;
			}
		}
	}
	//Explosion code
	if (parts[i].life == 1)
	{
		sim->create_part(i, x, y, PT_EXOT);
		parts[i].tmp2 = rand()%1000;
		int random = rand(), index;
		index = sim->create_part(-3,x+((random>>4)&3)-1,y+((random>>6)&3)-1,PT_ELEC);
		if (index != -1)
			parts[index].temp = 7000;
		index = sim->create_part(-3,x+((random>>8)&3)-1,y+((random>>10)&3)-1,PT_PHOT);
		if (index != -1)
			parts[index].temp = 7000;
		index = sim->create_part(-3,x+((random>>12)&3)-1,y+rand()%3-1,PT_BREC);
		if (index != -1)
			parts[index].temp = 7000;
		parts[i].temp=9000;
		sim->pv[y/CELL][x/CELL]=200;
	}
	//Neighbor check loop
	for (rx=-2; rx<3; rx++)
		for (ry=-2; ry<3; ry++)
			if (x+rx>=0 && y+ry>0 && x+rx<XRES && y+ry<YRES && (rx || ry))
			{
				r = pmap[y+ry][x+rx];
				if (!r)
					continue;
				//Melts into EXOT
				if ((r&0xFF)==PT_EXOT && !(rand()%250))
				{
					sim->create_part(i, x, y, PT_EXOT);
				}
				//Absorbs energy particles
				if ((sim->elements[r&0xFF].Properties & TYPE_ENERGY))
				{
					parts[i].tmp += 10;
					sim->kill_part(r>>8);
				}
				if ((r&0xFF)==PT_BOYL)
				{
					sim->part_change_type(i,x,y,PT_BVBR);
				}
			}
	transferProp(UPDATE_FUNC_SUBCALL_ARGS, offsetof(Particle, tmp));
	return 0;
}

//#TPT-Directive ElementHeader Element_VIBR static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VIBR::graphics(GRAPHICS_FUNC_ARGS)
{
	int gradient = cpart->tmp/10;
	if (gradient >= 100 || cpart->life)
	{
		*colr = abs(sin(exp((750.0f-cpart->life)/170)))*200;
		*colg = 255;
		*colb = abs(sin(exp((750.0f-cpart->life)/170)))*200;
		*firea = 90;
		*firer = *colr;
		*fireg = *colg;
		*fireb = *colb;
		*pixel_mode = PMODE_NONE;
		*pixel_mode |= FIRE_BLEND;
	}
	else if (gradient < 100)
	{
		*colr += (int)restrict_flt(gradient*2.0f,0,255);
		*colg += (int)restrict_flt(gradient*2.0f,0,175);
		*colb += (int)restrict_flt(gradient*2.0f,0,255);
		*firea = (int)restrict_flt(gradient*.6f,0,60);
		*firer = *colr/2;
		*fireg = *colg/2;
		*fireb = *colb/2;
		*pixel_mode |= FIRE_BLEND;
	}
	return 0;
}

Element_VIBR::~Element_VIBR() {}