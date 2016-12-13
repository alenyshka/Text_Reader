#if !defined _GLYPH_H_
#define _GLYPH_H_

class Glyph
{
public:
	virtual ~Glyph() {}
	virtual int getHeight()=0;
	virtual int getWidth()=0;
	virtual HDC draw(HDC hDc, int x0, int y0, HFONT font) = 0;
	/*const type_info &getType()
	{
		return typeid(this);
	}*/
};

#endif
