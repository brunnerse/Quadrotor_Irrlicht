#pragma once
#include <irrlicht.h>
#include "RingBuffer.h"

using namespace irr;


class Graph
{
	core::rect<s32> pos;
	video::SColor colorRect, colorFont;

	core::stringw caption;

	int width, height;
	int bufSize, numBuffers;
	RingBuffer<core::vector2df>** buffers;
	int nextIdx = 0, numElements = 0;
	float maxVal;
	gui::IGUIFont* font;

public:

	Graph(const wchar_t* caption, core::rect<s32> pos, float maxVal, int numBuffers, int bufSize,
		gui::IGUIFont* font)
	{
		this->caption = caption;
		this->font = font;
		this->maxVal = maxVal;
		this->pos = pos;
		this->bufSize = bufSize;
		this->numBuffers = numBuffers;
		this->buffers = new RingBuffer<core::vector2df>*[numBuffers];
		for (int i = 0; i < numBuffers; ++i)
			buffers[i] = new RingBuffer<core::vector2df>(bufSize);

		colorRect.set(150, 50, 50, 50);
		colorFont.set(255, 255, 255, 255);

		this->width = pos.LowerRightCorner.X - pos.UpperLeftCorner.X;
		this->height = pos.LowerRightCorner.Y - pos.UpperLeftCorner.Y;
	}
	
	~Graph() {
		for (int i = 0; i < numBuffers; ++i)
			delete buffers[i];
		delete buffers;
	}

	void addVal(int buffer, core::vector2df val) {
		buffers[buffer]->push(val);
	}



	virtual void render(video::IVideoDriver* driver)
	{
		driver->draw2DRectangle(colorRect, pos);
		font->draw(caption, pos, colorFont, true);
		video::SColor color;
		for (int i = 0; i < numBuffers; ++i) {
			color.set(255, 255 * (i == 0), 255 * (i == 1), 255 * (i == 2));
			int numVals = buffers[i]->getNumElements();
			if (numVals < 2)
				continue;
			float startVal = buffers[i]->get(0).X;
			float xSpan = buffers[i]->get(numVals - 1).X - startVal;
			for (int idx = numVals - 2; idx >= 0; --idx) {
				core::vector2d<s32> startPos, endPos;
				startPos.X = (s32)((buffers[i]->get(idx).X - startVal) / xSpan * width) + pos.UpperLeftCorner.X;
				startPos.Y = pos.LowerRightCorner.Y - (s32)(buffers[i]->get(idx).Y / maxVal * height);
				endPos.X = (s32)((buffers[i]->get(idx+1).X -startVal) / xSpan * width) + pos.UpperLeftCorner.X;
				endPos.Y = pos.LowerRightCorner.Y - (s32)(buffers[i]->get(idx+1).Y / maxVal * height);
				if (startPos.X < pos.UpperLeftCorner.X)
					break;
				//driver->draw2DRectangle(color, core::rect<s32>(startPos, endPos));
				driver->draw2DLine(startPos, endPos, color);
			}
		}
	}

};