#include "simulation/Elements.h"
#include <conio.h>
#include "inc/fmod.h"
FSOUND_SAMPLE* handle;
//#TPT-Directive ElementClass Element_SNDM PT_SNDM 167
Element_SNDM::Element_SNDM()
{
    Identifier = "DEFAULT_PT_SNDM";
    Name = "SNDM";
    Colour = PIXPACK(0x40A060);
    MenuVisible = 1;
    MenuSection = SC_ELEC;
    Enabled = 1;
    handle=FSOUND_Sample_Load (0,"my.mp3",0, 0, 0);
    Advection = 0.0f;
    AirDrag = 0.00f * CFDS;
    AirLoss = 0.90f;
    Loss = 0.00f;
    Collision = 0.0f;
    Gravity = 0.0f;
    Diffusion = 0.00f;
    HotAir = 0.000f	* CFDS;
    Falldown = 0;
    
    Flammable = 0;
    Explosive = 0;
    Meltable = 0;
    Hardness = 2;
    
    Weight = 100;
    
    Temperature = R_TEMP+0.0f	+273.15f;
    HeatConduct = 0;
    Description = "Sound Maker";
    
    State = ST_SOLID;
    Properties = TYPE_SOLID;
    
    LowPressure = IPL;
    LowPressureTransition = NT;
    HighPressure = 15.0f;
    HighPressureTransition = PT_METL;
    LowTemperature = ITL;
    LowTemperatureTransition = NT;
    HighTemperature = ITH;
    HighTemperatureTransition = NT;
    
    Update = &Element_SNDM::update;
    Graphics = &Element_SNDM::graphics;
}

//#TPT-Directive ElementHeader Element_WIFI static int update(UPDATE_FUNC_ARGS)
int Element_SNDM::update(UPDATE_FUNC_ARGS)
 {
	// Removing WIFI code broke element
		int r, rx, ry;
	parts[i].tmp = (int)((parts[i].temp-73.15f)/100+1);
	if (parts[i].tmp>=CHANNELS) parts[i].tmp = CHANNELS-1;
	else if (parts[i].tmp<0) parts[i].tmp = 0;
	for (rx=-1; rx<2; rx++)
		for (ry=-1; ry<2; ry++)
			if (x+rx>=0 && y+ry>0 && x+rx<XRES && y+ry<YRES && (rx || ry))
			{
				r = pmap[y+ry][x+rx];
				if (!r)
					continue;
				// wireless[][0] - whether channel is active on this frame
				// wireless[][1] - whether channel should be active on next frame
				if (sim->wireless[parts[i].tmp][0])
				{
					if ((r&0xFF)==PT_SPRK && parts[r>>8].ctype==PT_NSCN)
					{
						FSOUND_Sample_Free(handle);
						FSOUND_Close();
					}
				}
				else
				{
					if ((r&0xFF)==PT_SPRK && parts[r>>8].ctype!=PT_NSCN && parts[r>>8].life==1)
					{
						 
   FSOUND_PlaySound (0,handle);
					}
				}
			}
	return 0;
  
}


//#TPT-Directive ElementHeader Element_WIFI static int graphics(GRAPHICS_FUNC_ARGS)
int Element_SNDM::graphics(GRAPHICS_FUNC_ARGS)

{
	float frequency = 0.0628;
	int q = cpart->tmp;
	*colr = sin(frequency*q + 0) * 127 + 128;
	*colg = sin(frequency*q + 2) * 127 + 128;
	*colb = sin(frequency*q + 4) * 127 + 128;
	*pixel_mode |= EFFECT_DBGLINES;
	return 0;
}


Element_SNDM::~Element_SNDM() {}
