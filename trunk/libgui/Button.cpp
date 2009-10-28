#include "Button.h"
#include "GraphicsGX.h"
#include "IPLFont.h"
#include "Image.h"
#include "FocusManager.h"

namespace menu {

Button::Button(Image *image, char** label, float x, float y, float width, float height)
		: active(false),
		  selected(false),
		  normalImage(0),
		  selectedImage(0),
		  focusImage(0),
		  buttonText(label),
		  x(x),
		  y(y),
		  width(width),
		  height(height),
		  clickedFunc(0),
		  returnFunc(0)
{
	setType(TYPE_BUTTON);
	setNormalImage(image);
						//Focus color			Inactive color		  Active color			Selected color		  Label color
	GXColor colors[5] = {{255, 100, 100, 255}, {255, 255, 255,  70}, {255, 255, 255, 130}, {255, 255, 255, 255}, {255, 255, 255, 255}};
	setButtonColors(colors);
}

Button::~Button()
{
}

void Button::setActive(bool activeBool)
{
	active = activeBool;
}

bool Button::getActive()
{
	return active;
}

void Button::setSelected(bool selectedBool)
{
	selected = selectedBool;
}

void Button::setReturn(ButtonFunc newReturnFunc)
{
	returnFunc = newReturnFunc;
}

void Button::doReturn()
{
	if (returnFunc) returnFunc();
}

void Button::setClicked(ButtonFunc newClickedFunc)
{
	clickedFunc = newClickedFunc;
}

void Button::doClicked()
{
	if (clickedFunc) clickedFunc();
}

void Button::setText(char** strPtr)
{
	buttonText = strPtr;
}

void Button::setNormalImage(Image *image)
{
	normalImage = image;
}

void Button::setSelectedImage(Image *image)
{
	selectedImage = image;
}

void Button::setFocusImage(Image *image)
{
	focusImage = image;
}

void Button::drawComponent(Graphics& gfx)
{
//	printf("Button drawComponent\n");

	gfx.setColor(inactiveColor);

	//activate relevant texture
	if(active)
	{
		//draw normalImage with/without gray mask and with alpha test on
		//printf("Button Active\n");
		gfx.setColor(activeColor);
	}
	if(getFocus())
	{
		//draw focus indicator (extra border for button?)
		//printf("Button in Focus\n");
		gfx.setColor(focusColor);
	}
	//draw buttonLabel?

	gfx.enableBlending(true);
//	gfx.setTEV(GX_PASSCLR);
	gfx.setTEV(GX_MODULATE);

//	gfx.setColor(focusColor);
	gfx.setDepth(-10.0f);
	gfx.newModelView();
	gfx.loadModelView();
	gfx.loadOrthographic();

//	gfx.fillRect(x, y, width, height);
	normalImage->activateImage(GX_TEXMAP0);
	gfx.drawImage(0, x, y, width/2, height/2, 0.0, width/16.0, 0.0, height/16.0);
	gfx.drawImage(0, x+width/2, y, width/2, height/2, width/16.0, 0.0, 0.0, height/16.0);
	gfx.drawImage(0, x, y+height/2, width/2, height/2, 0.0, width/16.0, height/16.0, 0.0);
	gfx.drawImage(0, x+width/2, y+height/2, width/2, height/2, width/16.0, 0.0, height/16.0, 0.0);
//	gfx.drawImage(0, x, y, width, height, 0.0, 1.0, 0.0, 1.0);

	if (selected)
	{
		gfx.setColor(selectedColor);
		if(selectedImage) selectedImage->activateImage(GX_TEXMAP0);
		gfx.drawImage(0, x, y, width/2, height/2, 0.0, width/16.0, 0.0, height/16.0);
		gfx.drawImage(0, x+width/2, y, width/2, height/2, width/16.0, 0.0, 0.0, height/16.0);
		gfx.drawImage(0, x, y+height/2, width/2, height/2, 0.0, width/16.0, height/16.0, 0.0);
		gfx.drawImage(0, x+width/2, y+height/2, width/2, height/2, width/16.0, 0.0, height/16.0, 0.0);
	}

	if (buttonText)
	{
		IplFont::getInstance().drawInit(labelColor);
		IplFont::getInstance().drawString((int) (x+width/2), (int) (y+height/2), *buttonText, 1.0, true);
	}
}

Component* Button::updateFocus(int direction, int buttonsPressed)
{
	Component* newFocus = NULL;
	if(!getFocus() && active)
	{
		setFocus(true);
		newFocus = this;
	}
	else if (!getFocus() && !active)
	{
		//try to move direction, and if new component found send to focus manager and remove focus for current component
		if (getNextFocus(direction))
			newFocus = (getNextFocus(direction))->updateFocus(direction, buttonsPressed);
		else
			newFocus = getNextFocus(direction);
	}
	else
	{
		if (getNextFocus(direction))
			newFocus = (getNextFocus(direction))->updateFocus(direction, buttonsPressed);
		if (newFocus)
			setFocus(false);
		else
			newFocus = this;
	}
	if (newFocus == this)
	{
		//finally update button behavior based on buttons pressed
		if(buttonsPressed & Focus::ACTION_BACK)
			doReturn();
		else if (buttonsPressed & Focus::ACTION_SELECT)
			doClicked();
	}
	return newFocus;
}

void Button::setButtonColors(GXColor *colors)
{
	focusColor.r = colors[0].r;
	focusColor.g = colors[0].g;
	focusColor.b = colors[0].b;
	focusColor.a = colors[0].a;
	inactiveColor.r = colors[1].r;
	inactiveColor.g = colors[1].g;
	inactiveColor.b = colors[1].b;
	inactiveColor.a = colors[1].a;
	activeColor.r = colors[2].r;
	activeColor.g = colors[2].g;
	activeColor.b = colors[2].b;
	activeColor.a = colors[2].a;
	selectedColor.r = colors[3].r;
	selectedColor.g = colors[3].g;
	selectedColor.b = colors[3].b;
	selectedColor.a = colors[3].a;
	labelColor.r = colors[4].r;
	labelColor.g = colors[4].g;
	labelColor.b = colors[4].b;
	labelColor.a = colors[4].a;
}

} //namespace menu 