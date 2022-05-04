#pragma once

#include"DEResource.h"
#include<string>
#include"Color.h"
//#include"DERect.h"

typedef unsigned int GLuint;


class ResourceTexture : public Resource
{
public:
	ResourceTexture(std::string& _uid);
	~ResourceTexture();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;
	//DERect GetTextureChunk(DERect area);

public:
	int tWidth;
	int tHeight;

	GLuint textureID;
	Color color;

};


//struct DERect
//{
//	int x, y, w, h;
//	DERect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
//	DERect() : x(0), y(0), w(0), h(0) {}
//	bool Contains(DERect b);
//};

//bool DERect::Contains(DERect b)
//{
//    if ((b.x + b.w) <= (x + w)
//        && (b.x) >= (x)
//        && (b.y) >= (y)
//        && (b.y + b.h) <= (y + h)
//        )
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
