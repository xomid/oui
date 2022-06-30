#pragma once
#include "oui_button.h"

class OUI_API Curve
{
public:

	Curve()
	{
		value_lookup = new byte[256];
		points_lookup = new double[256];
	}

public:
	std::vector<double> xs;
	std::vector<double> ys;
	std::vector<double> ks;
	byte* value_lookup;
	double* points_lookup;
	int nCount, range;
	bool bSmooth, bIntact;
	double** AA;

public:
	void Stretch(int newrange);
	void Init(int w, int max);
	void FillData();
	void ConvertTo(int mode);
	void FillLookUp();
	double** zerosMat(int r, int c);
	void swapRows(double** m, int k, int l);
	void solve(double** A, int len);
	void getNaturalKs();
	int evalSpline(int x);
	double evalSpline(double x);
	void Sort();
	int Add(int x, int y);
	void Remove(int nIndex);
	void erase(int iIndex, int iCount);
	void Reset();
	void Insert(int nIndex, int x, int y);
	bool IsIntact();
	void Modify(int nIndex, double x, double y);
};


class OUI_API UICurve : public UIButton {
	Sheet img, *srcImage;
	Curve val_curve, red_curve, green_curve, blue_curve, * curve;
	bool bCanChangeCursor, bSuspend, bMoving, bCreated, bSmooth, bNew;
	int w, h, imode, iHover, iCursor, range, iLastSel;
	unsigned long gray_histo[256], b_histo[256], g_histo[256], r_histo[256];

public:
	void get_histo(Sheet* image);
	void set_source(Sheet* srcImage);
	void fill_image();
	void apply_theme(bool bInvalidate = true) override;
	void on_init() override;
	void on_resize(int width, int height) override;
	void on_update() override;
	void on_mouse_move(int x, int y, uint32_t flags) override;
	void on_mouse_down(int x, int y, uint32_t flags) override;
	void on_dbl_click(int x, int y, uint32_t flags) override;
	void on_mouse_up(int x, int y, uint32_t flags) override;
};



