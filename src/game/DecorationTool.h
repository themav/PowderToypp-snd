
#ifndef DECORATIONTOOL_H_
#define DECORATIONTOOL_H_

#include "Tool.h"

class DecorationTool: public Tool
{
public:
	enum ToolType { BlendAdd = DECO_ADD, BlendRemove = DECO_SUBTRACT, BlendMultiply = DECO_MULTIPLY, BlendDivide = DECO_DIVIDE, BlendSet = DECO_DRAW };

	ToolType decoMode;

	DecorationTool(ToolType decoMode_, string name, int r, int g, int b):
		Tool(0, name, r, g, b),
		decoMode(decoMode_)
	{
	}
	virtual ~DecorationTool() {}
	virtual void Draw(Simulation * sim, Brush * brush, ui::Point position){
		sim->ApplyDecorationPoint(position.X, position.Y, 1, 1, 24, 24, 24, 255, decoMode, brush);
	}
	virtual void DrawLine(Simulation * sim, Brush * brush, ui::Point position1, ui::Point position2) {
		sim->ApplyDecorationLine(position1.X, position1.Y, position2.X, position2.Y, 1, 1, 24, 24, 24, 255, decoMode, brush);
	}
	virtual void DrawRect(Simulation * sim, Brush * brush, ui::Point position1, ui::Point position2) {
		sim->ApplyDecorationBox(position1.X, position1.Y, position2.X, position2.Y, 24, 24, 24, 255, decoMode);
	}
	virtual void DrawFill(Simulation * sim, Brush * brush, ui::Point position) {

	}
};

#endif