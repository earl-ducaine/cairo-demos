// "cairo in motion" is a testbed for animated vector-drawing
// using cairo within a gtk+-environment
//
// Copyright (C) 2005 Mirco Mueller <macslow@bangang.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <math.h>
#include <iostream>
#include <svg-cairo.h>
#include <sys/time.h>

#include "cairo-samples.h"

CairoSamples::CairoSamples ()
{
	// initialize animation-variables
	m_center.x = 0.5f;
	m_center.y = 0.5f;
	m_point1.x = 0.1f;
	m_point1.y = 0.2f;
	m_point2.x = 0.5f;
	m_point2.y = 0.5f;
	m_point3.x = 0.8f;
	m_point3.y = 0.4f;
	m_point4.x = 0.2f;
	m_point4.y = 0.9f;
	m_fRadius = 0.25f;
	m_fAngle1 = 0.0f;
	m_fAngle2 = 360.0f;
	m_centerXStep = 0.0025f;
	m_centerYStep = 0.0025f;
	m_point1XStep = -0.0025f;
	m_point1YStep = -0.0025f;
	m_point2XStep = 0.0025f;
	m_point2YStep = -0.0025f;
	m_point2XStep = 0.0025f;
	m_point3YStep = -0.0025f;
	m_point4XStep = 0.0025f;
	m_point4YStep = -0.0025f;
	m_fRadiusStep = 0.0025f;
	m_fAngle1Step = 0.5f;
	m_fAngle2Step = -1.5f;

	// setup sample-entry list
	Entry* pEntry = new Entry;
	pEntry->name = "curve_rectangle";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "clock";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "fill_and_stroke";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "fill_and_stroke2";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "fontmania";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "gradient";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "image";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "image_pattern";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "libsvg";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_arc";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_arc_negative";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_cairo_logo";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_clip";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_clip_image";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_curve_to";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_gradient";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_gradient2";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "moving_zini";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_add";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_atop";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_atop_reverse";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_in";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_in_reverse";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_out";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_out_reverse";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_over";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_over_reverse";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_saturate";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "operator_xor";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "path";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "pattern_fill";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "set_line_cap";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "set_line_join";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "text_align_center";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "text";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "text_extents";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "xxx_clip_rectangle";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "xxx_dash";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "xxx_long_lines";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "xxx_multi_segment_caps";
	m_sampleList.push_back (*pEntry);
	pEntry->name = "xxx_self_intersect";
	m_sampleList.push_back (*pEntry);
}

CairoSamples::~CairoSamples ()
{
}

void CairoSamples::do_animation_step (unsigned long ulMilliSeconds)
{
	srand (42);

	m_ulMilliSeconds = ulMilliSeconds;

	// update vars
	update_var (&m_center.x,
				0.0f,
				1.0f,
				&m_centerXStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_center.y,
				0.0f,
				1.0f,
				&m_centerYStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_point1.x,
				0.0f,
				1.0f,
				&m_point1XStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_point1.y,
				0.0f,
				1.0f,
				&m_point1YStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_point2.x,
				0.0f,
				1.0f,
				&m_point2XStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_point2.y,
				0.0f,
				1.0f,
				&m_point2YStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_point3.x,
				0.0f,
				1.0f,
				&m_point3XStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_point3.y,
				0.0f,
				1.0f,
				&m_point3YStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_point4.x,
				0.0f,
				1.0f,
				&m_point4XStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_point4.y,
				0.0f,
				1.0f,
				&m_point4YStep,
				(double) rand() * 0.025 / RAND_MAX,
				(double) rand() * -0.025 / RAND_MAX);

	update_var (&m_fRadius,
				0.05f,
				0.5f,
				&m_fRadiusStep,
				(double) rand() * 0.0125 / RAND_MAX,
				(double) rand() * -0.0125 / RAND_MAX);

	update_var (&m_fAngle1,
				0.0f,
				360.0f,
				&m_fAngle1Step,
				0.5f,
				-1.0f);

	update_var (&m_fAngle2,
				0.0f,
				360.0f,
				&m_fAngle2Step,
				2.0f,
				-0.75f);
}

void CairoSamples::set_context (Cairo::Context* pCairoContext)
{
	m_pCairoContext = pCairoContext;
}

void CairoSamples::normalize_canvas (double fWidth, double fHeight) const
{
	m_pCairoContext->scale(fWidth, fHeight);
}

void CairoSamples::set_bg_svg (std::string strSVGFileName)
{
	unsigned int uiWidth;
	unsigned int uiHeight;
	svg_cairo_t* pSVGContext;

	svg_cairo_create (&pSVGContext);
	svg_cairo_parse (pSVGContext, strSVGFileName.c_str ());
	svg_cairo_get_size (pSVGContext, &uiWidth, &uiHeight);
	m_pCairoContext->save ();
	m_pCairoContext->scale (1.0f / uiWidth, 1.0f / uiHeight);
	svg_cairo_render (pSVGContext, m_pCairoContext->cobj ());
	m_pCairoContext->restore ();
	svg_cairo_destroy (pSVGContext);
}

bool CairoSamples::do_sample (Sample sample)
{
	bool bResult = false;

	switch (sample)
	{
		case CurveRectangle :
			bResult = curve_rectangle ();
		break;

		case Clock :
			bResult = clock ();
		break;

		case FillAndStroke :
			bResult = fill_and_stroke ();
		break;

		case FillAndStroke2 :
			bResult = fill_and_stroke2 ();
		break;

		case Fontmania :
			bResult = fontmania ();
		break;

		case Gradient :
			bResult = gradient ();
		break;

		case Image :
			//bResult = image ();
			bResult = sample_fallback ("Sample disabled");
		break;

		case ImagePattern :
			//bResult = image_pattern ();
			bResult = sample_fallback ("Sample disabled");
		break;

		case LibSVG :
			bResult = libsvg ();
		break;

		case MovingArc :
			bResult = moving_arc ();
		break;

		case MovingArcNegative :
			bResult = moving_arc_negative ();
		break;

		case MovingCairoLogo :
			bResult = moving_cairo_logo ();
		break;

		case MovingClip :
			bResult = moving_clip ();
		break;

		case MovingClipImage :
			//bResult = moving_clip_image ();
			bResult = sample_fallback ("Sample disabled");
		break;

		case MovingCurveTo :
			bResult = moving_curve_to ();
		break;

		case MovingGradient :
			bResult = moving_gradient ();
		break;

		case MovingGradient2 :
			bResult = moving_gradient2 ();
		break;

		case MovingZini:
			bResult = moving_zini ();
		break;

		case OperatorAdd :
			bResult = operator_add ();
		break;

		case OperatorAtop :
			bResult = operator_atop ();
		break;

		case OperatorAtopReverse :
			bResult = operator_atop_reverse ();
		break;

		case OperatorIn :
			bResult = operator_in ();
		break;

		case OperatorInReverse :
			bResult = operator_in_reverse ();
		break;

		case OperatorOut :
			bResult = operator_out ();
		break;

		case OperatorOutReverse :
			bResult = operator_out_reverse ();
		break;

		case OperatorOver :
			bResult = operator_over ();
		break;

		case OperatorOverReverse :
			bResult = operator_over_reverse ();
		break;

		case OperatorSaturate :
			bResult = operator_saturate ();
		break;

		case OperatorXor :
			bResult = operator_xor ();
		break;

		case Path :
			bResult = path ();
		break;

		case PatternFill :
			bResult = pattern_fill ();
		break;

		case SetLineCap :
			bResult = set_line_cap ();
		break;

		case SetLineJoin :
			bResult = set_line_join ();
		break;

		case TextAlignCenter :
			bResult = text_align_center ();
		break;

		case Text :
			bResult = text ();
		break;

		case TextExtents :
			bResult = text_extents ();
		break;

		case XxxClipRectangle :
			bResult = xxx_clip_rectangle ();
		break;

		case XxxDash :
			bResult = xxx_dash ();
		break;

		case XxxLongLines :
			bResult = xxx_long_lines ();
		break;

		case XxxMultiSegmentCaps :
			bResult = xxx_multi_segment_caps ();
		break;

		case XxxSelfIntersect :
			bResult = xxx_self_intersect ();
		break;

		default :
			bResult = sample_fallback ("Sample not found");
		break;
	}

	return bResult;
}

void CairoSamples::set_cap_style (Cairo::LineCap cairoCapStyle)
{
	m_cairoCapStyle = cairoCapStyle;
}

void CairoSamples::set_join_style (Cairo::LineJoin cairoJoinStyle)
{
	m_cairoJoinStyle = cairoJoinStyle;
}

void CairoSamples::set_dash_style (DashStyle dashStyle)
{
	m_dashStyle = dashStyle;
}

void CairoSamples::set_operator (Cairo::Operator cairoOperator)
{
	m_cairoOperator = cairoOperator;
}

void CairoSamples::set_extend_mode (Cairo::Extend cairoExtendMode)
{
	m_cairoExtendMode = cairoExtendMode;
}

void CairoSamples::set_filter_mode (Cairo::Filter cairoFilterMode)
{
	m_cairoFilterMode = cairoFilterMode;
}

void CairoSamples::set_line_width (double fLineWidth)
{
	m_fLineWidth = fLineWidth;
}

void CairoSamples::set_bg_color (Gdk::Color bgColor)
{
	m_bgColor = bgColor;
}

void CairoSamples::set_font_family (std::string strFontFamily)
{
	m_strFontFamily = strFontFamily;
}

void CairoSamples::set_font_size (double fFontSize)
{
	m_fFontSize = fFontSize;
}

void CairoSamples::set_font_weight (Cairo::FontWeight cairoFontWeight)
{
	m_cairoFontWeight = cairoFontWeight;
}

void CairoSamples::set_font_slant (Cairo::FontSlant cairoFontSlant)
{
	m_cairoFontSlant = cairoFontSlant;
}

void CairoSamples::set_some_text (std::string strSomeText)
{
	m_strSomeText = strSomeText;
}

void CairoSamples::set_image_file_name (std::string strFileName)
{
	m_strImageFileName = strFileName;
}

void CairoSamples::set_svg_file_name (std::string strFileName)
{
	m_strSvgFileName = strFileName;
}

void CairoSamples::add_samples_to_list_store (SampleListStore* pSampleListStore)
{
	if (!pSampleListStore)
		return;

	for (std::vector<Entry>::iterator iter = m_sampleList.begin ();
		 iter != m_sampleList.end ();
		 iter++)
	{
		pSampleListStore->add_item ((*iter).name);
	}
}

std::string CairoSamples::get_sample_name (Sample sample)
{
	return m_sampleList[sample].name;
}

void CairoSamples::update_var (double* pfValue,
							   double fLowerLimit,
							   double fUpperLimit,
							   double* pfStep,
							   double fGrow,
							   double fShrink)
{
	if (*pfValue <= fLowerLimit)
	{
		*pfValue = fLowerLimit;
		*pfStep = fGrow;
	}

	if (*pfValue >= fUpperLimit)
	{
		*pfValue = fUpperLimit;
		*pfStep = fShrink;
	}

	*pfValue += *pfStep;
}

HandlePoint CairoSamples::get_center ()
{
	return m_center;
}

HandlePoint CairoSamples::get_point1 ()
{
	return m_point1;
}

HandlePoint CairoSamples::get_point2 ()
{
	return m_point2;
}

HandlePoint CairoSamples::get_point3 ()
{
	return m_point3;
}

HandlePoint CairoSamples::get_point4 ()
{
	return m_point4;
}

double CairoSamples::get_radius ()
{
	return m_fRadius;
}

double CairoSamples::get_angle_1 ()
{
	return m_fAngle1;
}

double CairoSamples::get_angle_2 ()
{
	return m_fAngle2;
}

void CairoSamples::draw_background ()
{
	int iX;
	int iY;
	bool bToggle = false;

	for (iX = 0; iX < 20; iX++)
	{
		bToggle = !bToggle;
		for (iY = 0; iY < 20; iY++)
		{
			if (bToggle)
			{
				m_pCairoContext->set_source_rgb (m_bgColor.get_red_p (),
												 m_bgColor.get_green_p (),
												 m_bgColor.get_blue_p ());
				bToggle = !bToggle;
			}
			else
			{
				m_pCairoContext->set_source_rgb (m_bgColor.get_red_p() - 0.1f,
												 m_bgColor.get_green_p() - 0.1f,
												 m_bgColor.get_blue_p() - 0.1f);
				bToggle = !bToggle;
			}

			m_pCairoContext->rectangle (iX * 0.05f,
										iY * 0.05f,
										0.05f,
										0.05f);
			m_pCairoContext->fill ();
		}
	}
}

void CairoSamples::draw_background (Gdk::Color color)
{
	m_pCairoContext->set_source_rgba (color.get_red_p (),
									  color.get_green_p (),
									  color.get_blue_p (),
									  1.0f);
	m_pCairoContext->rectangle (0.0f, 0.0f, 1.0f, 1.0f);
	m_pCairoContext->fill ();
}

void CairoSamples::draw_handle (HandlePoint* pPoint)
{
	if (!pPoint)
		return;

	draw_handle (pPoint->x, pPoint->y);
}

void CairoSamples::draw_handle (double fX, double fY)
{
	double fRadius = 0.01f;
	std::valarray<double> dashes (1);

	if (!m_pCairoContext)
		return;

	dashes[0] = 1.0f;

	m_pCairoContext->set_dash (dashes, 0.0f);
	m_pCairoContext->set_line_width (0.002f);
	m_pCairoContext->set_line_cap (CAIRO_LINE_CAP_BUTT);
	m_pCairoContext->set_line_join (CAIRO_LINE_JOIN_ROUND);

	/* draw white/green-ish outline */
	m_pCairoContext->set_source_rgb (0.9f, 1.0f, 0.8f);
	m_pCairoContext->arc (fX, fY, fRadius, 0.0f, 2.0f * M_PI);
	m_pCairoContext->stroke ();

	/* draw grey/transparent center */
	m_pCairoContext->set_source_rgba (0.6f, 0.6f, 0.6f, 0.5f);
	m_pCairoContext->arc (fX, fY, fRadius, 0.0f, 2.0f * M_PI);
	m_pCairoContext->fill ();
}

bool CairoSamples::sample_fallback (std::string strMessage)
{
	if (!m_pCairoContext)
		return false;

	m_pCairoContext->select_font_face ("Sans",
									   CAIRO_FONT_SLANT_NORMAL,
									   CAIRO_FONT_WEIGHT_NORMAL);
	m_pCairoContext->set_font_size (0.115f);

	Gdk::Color white ("#ffffff");
	draw_background (white);

	m_pCairoContext->set_source_rgb (1.0f, 0.0f, 0.0f);
	m_pCairoContext->move_to (0.04f, 0.53f);
	m_pCairoContext->show_text (strMessage.c_str ());

	return true;
}

bool CairoSamples::curve_rectangle ()
{
	if (!m_pCairoContext)
		return false;

	double x0 = 0.1f;
	double y0 = 0.1f;
	double rect_width  = 0.8f;
	double rect_height = 0.8f;
	double radius = 0.4f;
	double x1;
	double y1;

	x1 = x0 + rect_width;
	y1 = y0 + rect_height;

	draw_background ();

	m_pCairoContext->set_line_width (m_fLineWidth);

	if (!rect_width || !rect_height)
		return false;

	if (rect_width / 2.0f < radius)
	{
		if (rect_height / 2.0f < radius)
		{
			m_pCairoContext->move_to (x0, (y0 + y1) / 2.0f);
			m_pCairoContext->curve_to (x0 ,y0, x0, y0, (x0 + x1) / 2.0f, y0);
			m_pCairoContext->curve_to (x1, y0, x1, y0, x1, (y0 + y1) / 2.0f);
			m_pCairoContext->curve_to (x1, y1, x1, y1, (x1 + x0) / 2.0f, y1);
			m_pCairoContext->curve_to (x0, y1, x0, y1, x0, (y0 + y1) / 2.0f);
		}
		else
		{
			m_pCairoContext->move_to (x0, y0 + radius);
			m_pCairoContext->curve_to (x0 ,y0, x0, y0, (x0 + x1) / 2.0f, y0);
			m_pCairoContext->curve_to (x1, y0, x1, y0, x1, y0 + radius);
			m_pCairoContext->line_to (x1 , y1 - radius);
			m_pCairoContext->curve_to (x1, y1, x1, y1, (x1 + x0) / 2.0f, y1);
			m_pCairoContext->curve_to (x0, y1, x0, y1, x0, y1- radius);
		}
	}
	else
	{
		if (rect_height / 2.0f < radius)
		{
			m_pCairoContext->move_to (x0, (y0 + y1) / 2.0f);
			m_pCairoContext->curve_to (x0 , y0, x0 , y0, x0 + radius, y0);
			m_pCairoContext->line_to (x1 - radius, y0);
			m_pCairoContext->curve_to (x1, y0, x1, y0, x1, (y0 + y1) / 2.0f);
			m_pCairoContext->curve_to (x1, y1, x1, y1, x1 - radius, y1);
			m_pCairoContext->line_to (x0 + radius, y1);
			m_pCairoContext->curve_to (x0, y1, x0, y1, x0, (y0 + y1) / 2.0f);
	    }
		else
		{
			m_pCairoContext->move_to (x0, y0 + radius);
			m_pCairoContext->curve_to (x0 , y0, x0 , y0, x0 + radius, y0);
			m_pCairoContext->line_to (x1 - radius, y0);
			m_pCairoContext->curve_to (x1, y0, x1, y0, x1, y0 + radius);
			m_pCairoContext->line_to (x1 , y1 - radius);
			m_pCairoContext->curve_to (x1, y1, x1, y1, x1 - radius, y1);
			m_pCairoContext->line_to (x0 + radius, y1);
			m_pCairoContext->curve_to (x0, y1, x0, y1, x0, y1- radius);
	    }
	}
	m_pCairoContext->close_path ();

	m_pCairoContext->set_source_rgb (0.5f, 0.5f, 1.0f);
	m_pCairoContext->fill_preserve ();
	m_pCairoContext->set_source_rgba (0.5f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::clock ()
{
	int i;
	int iSeconds;
	int iMinutes;
	int iHours;
	static time_t timeOfDay;
	struct tm* pTime;

	if (!m_pCairoContext)
		return false;

	draw_background ();

	m_pCairoContext->save ();
	m_pCairoContext->translate (0.5f, 0.5f);
	m_pCairoContext->scale (0.4f, 0.4f);
	m_pCairoContext->rotate (-M_PI/2.0f);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->set_line_width (3.0f/60.0f);
	m_pCairoContext->set_line_cap (CAIRO_LINE_CAP_ROUND);
	m_pCairoContext->set_line_join (CAIRO_LINE_JOIN_ROUND);

	// draw hour-marks
	m_pCairoContext->save ();
	m_pCairoContext->new_path ();
	for (i = 0; i < 12; i++)
	{
		m_pCairoContext->rotate (M_PI/6.0f);
		m_pCairoContext->move_to (2.0f/3.0f, 0.0f);
		m_pCairoContext->line_to (4.0f/5.0f, 0.0f);
	}
	m_pCairoContext->close_path ();
	m_pCairoContext->stroke ();
	m_pCairoContext->restore ();

	// draw minute-marks
	m_pCairoContext->save ();
	m_pCairoContext->set_line_width (1.0f/45.0f);
	m_pCairoContext->new_path ();
	for (i = 0; i < 60; i++)
	{
		if (i%5 != 0)
		{
			m_pCairoContext->move_to(4.0f/5.0f - 1.0f/50.0f,0.0f);
			m_pCairoContext->line_to(4.0f/5.0f,0.0f);
		}
		m_pCairoContext->rotate(M_PI/30.0f);
	}
	m_pCairoContext->close_path ();
	m_pCairoContext->stroke ();
	m_pCairoContext->restore ();

	// if absolutely now clue how to do this determination of the current time
	// only via the classes and methods provided by glibmm/gtkmm, I would really
	// welcome anybody enlighten me on this
	time (&timeOfDay);
	pTime = localtime (&timeOfDay);
	iSeconds = pTime->tm_sec;
	iMinutes = pTime->tm_min;
	iHours   = pTime->tm_hour;

	iHours = iHours >= 12 ? iHours - 12 : iHours;

	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);

	// draw hour-hand
	m_pCairoContext->save ();
	m_pCairoContext->rotate ((M_PI/6.0f)*iHours + (M_PI/360.0f)*iMinutes + (M_PI/21600.0f)*iSeconds);
	m_pCairoContext->set_line_width (1.0f/15.0f);
	m_pCairoContext->new_path ();
	m_pCairoContext->move_to(-2.0f/15.0f, 0.0f);
	m_pCairoContext->line_to(7.0f/15.0f, 0.0f);
	m_pCairoContext->close_path ();
	m_pCairoContext->stroke ();
	m_pCairoContext->restore ();

	// draw minute-hand
	m_pCairoContext->save ();
	m_pCairoContext->rotate ( (M_PI/30)*iMinutes + (M_PI/1800)*iSeconds);
	m_pCairoContext->set_line_width (1.0f/30.0f);
	m_pCairoContext->new_path ();
	m_pCairoContext->move_to (-16.0f/75.0f, 0.0f);
	m_pCairoContext->line_to (2.0f/3.0f, 0.0f);
	m_pCairoContext->close_path ();
	m_pCairoContext->stroke ();
	m_pCairoContext->restore ();
  
	// draw second-hand
	m_pCairoContext->save ();
	m_pCairoContext->rotate (iSeconds * M_PI/30.0f);
	m_pCairoContext->set_source_rgb (0.82f, 0.0f, 0.0f);
	m_pCairoContext->set_line_width (1.0f / 50.0f);
	m_pCairoContext->new_path ();
	m_pCairoContext->move_to (-1.0f/5.0f, 0.0f);
	m_pCairoContext->line_to (3.0f/5.0f ,0.0f);
	m_pCairoContext->close_path ();
	m_pCairoContext->stroke ();
	m_pCairoContext->new_path ();
	m_pCairoContext->arc (0.0f, 0.0f, 1.0f/20.0f, 0.0f, M_PI * 2.0f);
	m_pCairoContext->fill ();
	m_pCairoContext->new_path ();
	m_pCairoContext->arc (32.0f/50.0f, 0.0f, 1.0f/35.0f, 0.0f, M_PI * 2.0f);
	m_pCairoContext->stroke ();
	m_pCairoContext->set_line_width (1.0f / 100.0f);
	m_pCairoContext->move_to (10.0f/15.0f, 0.0f);
	m_pCairoContext->line_to (11.0f/15.0f, 0.0f);
	m_pCairoContext->stroke ();
	m_pCairoContext->set_line_width (1.0f/ 50.0f);
	m_pCairoContext->set_source_rgb (0.31f, 0.31f, 0.31f);
	m_pCairoContext->arc (0.0f, 0.0f, 1.0f/25.0f, 0.0f, M_PI * 2);
	m_pCairoContext->close_path ();
	m_pCairoContext->fill ();
	m_pCairoContext->restore ();

	m_pCairoContext->new_path ();
	m_pCairoContext->set_line_width (10.0f/75.0f);
	m_pCairoContext->set_source_rgb (0.19f, 0.37f, 0.63f);
	m_pCairoContext->arc (0.0f, 0.0f, 142.0f/150.0f, 0.0f, M_PI * 2.0f);
	m_pCairoContext->close_path ();
	m_pCairoContext->stroke ();

	m_pCairoContext->restore ();

	return true;
}

bool CairoSamples::fill_and_stroke ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->move_to (0.5f, 0.1f);
	m_pCairoContext->line_to (0.9f, 0.9f);
	m_pCairoContext->rel_line_to (-0.4f, 0.0f);
	m_pCairoContext->curve_to (0.2f, 0.9f, 0.2f, 0.5f, 0.5f, 0.5f);
	m_pCairoContext->close_path ();

	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->fill_preserve ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::fill_and_stroke2 ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->move_to (0.5f, 0.1f);
	m_pCairoContext->line_to (0.9f, 0.9f);
	m_pCairoContext->rel_line_to (-0.4f, 0.0f);
	m_pCairoContext->curve_to (0.2f, 0.9f, 0.2f, 0.5f, 0.5f, 0.5f);
	m_pCairoContext->close_path ();

	m_pCairoContext->move_to (0.25f, 0.1f);
	m_pCairoContext->rel_line_to (0.2f, 0.2f);
	m_pCairoContext->rel_line_to (-0.2f, 0.2f);
	m_pCairoContext->rel_line_to (-0.2f, -0.2f);
	m_pCairoContext->close_path ();

	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->fill_preserve ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::fontmania ()
{
	draw_background ();

	std::string theText = "Fontmania";

	// set font-attributes
	m_pCairoContext->select_font_face ("Times New Roman",
									   CAIRO_FONT_SLANT_ITALIC,
									   CAIRO_FONT_WEIGHT_NORMAL);
	m_pCairoContext->set_font_size (0.6f);

	// draw the solid black text
	m_pCairoContext->set_source_rgba (0.0f, 0.0f, 0.0f, 0.15f);
	m_pCairoContext->move_to (0.05f, 0.6f);
	m_pCairoContext->show_text (theText);

	// set font-attributes
	m_pCairoContext->select_font_face ("Arial Black",
									   CAIRO_FONT_SLANT_NORMAL,
									   CAIRO_FONT_WEIGHT_NORMAL);
	m_pCairoContext->set_font_size (0.1f);

	// draw the solid black text
	m_pCairoContext->set_source_rgba (0.0f, 0.0f, 0.0f, 1.0f);
	m_pCairoContext->move_to (0.4f, 0.6f);
	m_pCairoContext->show_text (theText);

	// set font-attributes
	m_pCairoContext->select_font_face ("Verdana",
									   CAIRO_FONT_SLANT_NORMAL,
									   CAIRO_FONT_WEIGHT_NORMAL);
	m_pCairoContext->set_font_size (0.15f);

	// draw the solid black text
	m_pCairoContext->set_source_rgba (0.0f, 0.0f, 0.0f, 0.25f);
	m_pCairoContext->move_to (0.1f, 0.6f);
	m_pCairoContext->show_text (theText);

	// set font-attributes
	m_pCairoContext->select_font_face ("Impact",
									   CAIRO_FONT_SLANT_NORMAL,
									   CAIRO_FONT_WEIGHT_NORMAL);
	m_pCairoContext->set_font_size (0.2f);

	// draw the solid black text
	m_pCairoContext->set_source_rgba (0.0f, 0.0f, 0.0f, 0.35f);
	m_pCairoContext->move_to (0.3f, 0.6f);
	m_pCairoContext->show_text (theText);

	return true;
}

bool CairoSamples::gradient ()
{
	if (!m_pCairoContext)
		return false;

	Cairo::Pattern pattern = Cairo::Pattern::create_linear (0.0f, 0.0f,  0.0f, 1.0f);
	pattern.add_color_stop_rgba (1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	pattern.add_color_stop_rgba (0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	m_pCairoContext->rectangle (0.0f, 0.0f, 1.0f, 1.0f);
	m_pCairoContext->set_source (pattern);
	m_pCairoContext->fill ();

	pattern = Cairo::Pattern::create_radial (0.45f, 0.4f, 0.1f, 0.4f,  0.4f, 0.5f);
	pattern.add_color_stop_rgba (0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	pattern.add_color_stop_rgba (1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	m_pCairoContext->set_source (pattern);
	m_pCairoContext->arc (0.5f, 0.5f, 0.3f, 0.0f, 2.0f * M_PI);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::image ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();
	std::string bla;
	bla.assign (m_strImageFileName);

	int iWidth;
	int iHeight;
	Cairo::Surface image = Cairo::Surface::create (CAIRO_FORMAT_RGB24, 1, 1);
	//image.create_from_png (bla.c_str ());
	iWidth = image.get_width ();
	iHeight = image.get_height ();

	m_pCairoContext->translate (0.5f, 0.5f);
	m_pCairoContext->rotate (45.0f * M_PI / 180.0f);
	m_pCairoContext->scale (1.0f / iWidth, 1.0f / iHeight);
	m_pCairoContext->translate (-0.5f * iWidth, -0.5f * iHeight);

	m_pCairoContext->set_source_surface (image, 0.0f, 0.0f);
	m_pCairoContext->paint ();

	return true;
}

bool CairoSamples::image_pattern ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	int iWidth;
	int iHeight;
	Cairo::Surface image = Cairo::Surface::create (CAIRO_FORMAT_RGB24, 1, 1);
	Cairo::Pattern pattern = Cairo::Pattern::create_for_surface (image.cobj ());
	cairo_matrix_t matrix;

	//image.create_from_png (m_strImageFileName.c_str ());
	iWidth = image.get_width ();
	iHeight = image.get_height ();

	pattern.set_extend (m_cairoExtendMode);
	pattern.set_filter (m_cairoFilterMode);
	m_pCairoContext->translate (0.5f, 0.5f);
	m_pCairoContext->rotate (M_PI / 4.0f);
	m_pCairoContext->scale (1.0f / sqrt (2.0f), 1.0f / sqrt (2.0f));
	m_pCairoContext->translate (- 0.5f, - 0.5f);

	cairo_matrix_init_scale (&matrix, iWidth * 5.0f, iHeight * 5.0f); 
	pattern.set_matrix (matrix);

	m_pCairoContext->set_source (pattern);

	m_pCairoContext->rectangle (0.0f, 0.0f, 1.0f, 1.0f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::libsvg ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	svg_cairo_t* pSVGContext;
	unsigned int uiWidth;
	unsigned int uiHeight;

	svg_cairo_create (&pSVGContext);

	svg_cairo_parse (pSVGContext, m_strSvgFileName.c_str ());
	svg_cairo_get_size (pSVGContext, &uiWidth, &uiHeight);
	m_pCairoContext->scale (1.0f / uiWidth, 1.0f / uiHeight);
	svg_cairo_render (pSVGContext, m_pCairoContext->cobj ());

	svg_cairo_destroy (pSVGContext);

	return true;
}

bool CairoSamples::moving_arc ()
{
	if (!m_pCairoContext)
		return false;

	HandlePoint center = get_center ();
	double fRadius = get_radius ();
	double radiantAngle1 = get_angle_1 () * (M_PI/180.0);
	double radiantAngle2 = get_angle_2 () * (M_PI/180.0);
	HandlePoint firstHandle = { center.x + fRadius * cos (radiantAngle1),
							    center.y + fRadius * sin (radiantAngle1) };
	HandlePoint secondHandle = { center.x + fRadius * cos (radiantAngle2),
								 center.y + fRadius * sin (radiantAngle2)};

	draw_background ();

	// draw black arc
	m_pCairoContext->set_line_width(m_fLineWidth);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->arc (center.x,
						  center.y,
						  fRadius,
						  radiantAngle1,
						  radiantAngle2);
	m_pCairoContext->stroke ();

	// draw the two "helper"-lines
	m_pCairoContext->set_source_rgba (1.0f, 0.2f, 0.2f, 0.6f);
	m_pCairoContext->set_line_width (0.005f);
	m_pCairoContext->move_to (center.x, center.y);
	m_pCairoContext->line_to (firstHandle.x, firstHandle.y);
	m_pCairoContext->move_to (center.x, center.y);
	m_pCairoContext->line_to (secondHandle.x, secondHandle.y);
	m_pCairoContext->stroke ();

	// draw the three handle-points */
	draw_handle (&firstHandle);
	draw_handle (&secondHandle);
	draw_handle (&center);

	return true;
}

bool CairoSamples::moving_arc_negative ()
{
	if (!m_pCairoContext)
		return false;

	HandlePoint center = get_center ();
	double fRadius = get_radius ();
	double radiantAngle1 = get_angle_1 () * (M_PI / 180.0f);
	double radiantAngle2 = get_angle_2 () * (M_PI / 180.0f);
	HandlePoint firstHandle = { center.x + fRadius * cos (radiantAngle1),
							    center.y + fRadius * sin (radiantAngle1) };
	HandlePoint secondHandle = { center.x + fRadius * cos (radiantAngle2),
								 center.y + fRadius * sin (radiantAngle2)};

	draw_background ();

	// draw black arc
	m_pCairoContext->set_line_width(m_fLineWidth);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->arc_negative (center.x,
								   center.y,
								   fRadius,
								   radiantAngle1,
								   radiantAngle2);
	m_pCairoContext->stroke ();

	// draw the two "helper"-lines
	m_pCairoContext->set_source_rgba (1.0f, 0.2f, 0.2f, 0.6f);
	m_pCairoContext->set_line_width (0.005f);
	m_pCairoContext->move_to (center.x, center.y);
	m_pCairoContext->line_to (firstHandle.x, firstHandle.y);
	m_pCairoContext->move_to (center.x, center.y);
	m_pCairoContext->line_to (secondHandle.x, secondHandle.y);
	m_pCairoContext->stroke ();

	// draw the three handle-points */
	draw_handle (&firstHandle);
	draw_handle (&secondHandle);
	draw_handle (&center);

	return true;
}

bool CairoSamples::moving_cairo_logo ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	svg_cairo_t* pSVGContext;
	unsigned int uiWidth;
	unsigned int uiHeight;
	cairo_matrix_t matrix;
	HandlePoint offset = get_center ();
	
	svg_cairo_create (&pSVGContext);
	svg_cairo_parse (pSVGContext, "./cairo_logo.svg");
	svg_cairo_get_size (pSVGContext, &uiWidth, &uiHeight);
	m_pCairoContext->scale (1.0f / uiWidth, 1.0f / uiHeight);

	cairo_matrix_init_identity (&matrix);
	cairo_matrix_translate (&matrix,
							uiWidth * offset.x,
							uiHeight * offset.y - 160.0f);
	cairo_matrix_rotate (&matrix, get_angle_1 () * (M_PI/180.0f));
	cairo_matrix_scale (&matrix, 0.5f, 0.5f);
	m_pCairoContext->set_matrix (matrix);
	m_pCairoContext->paint_with_alpha (0.1f);
	svg_cairo_render (pSVGContext, m_pCairoContext->cobj ());

	cairo_matrix_init_identity (&matrix);
	cairo_matrix_translate (&matrix,
							uiWidth * offset.x,
							uiHeight * offset.y - 120.0f);
	cairo_matrix_rotate (&matrix, (5.0f + get_angle_1 ()) * (M_PI/180.0f));
	cairo_matrix_scale (&matrix, 0.5f, 0.5f);
	m_pCairoContext->set_matrix (matrix);
	m_pCairoContext->paint_with_alpha (0.2f);
	svg_cairo_render (pSVGContext, m_pCairoContext->cobj ());

	cairo_matrix_init_identity (&matrix);
	cairo_matrix_translate (&matrix,
							uiWidth * offset.x,
							uiHeight * offset.y - 80.0f);
	cairo_matrix_rotate (&matrix, (10.0f + get_angle_1 ()) * (M_PI/180.0f));
	cairo_matrix_scale (&matrix, 0.5f, 0.5f);
	m_pCairoContext->set_matrix (matrix);
	m_pCairoContext->paint_with_alpha (0.3f);
	svg_cairo_render (pSVGContext, m_pCairoContext->cobj ());

	cairo_matrix_init_identity (&matrix);
	cairo_matrix_translate (&matrix,
							uiWidth * offset.x,
							uiHeight * offset.y - 40.0f);
	cairo_matrix_rotate (&matrix, (15.0f + get_angle_1 ()) * (M_PI/180.0f));
	cairo_matrix_scale (&matrix, 0.5f, 0.5f);
	m_pCairoContext->set_matrix (matrix);
	m_pCairoContext->paint_with_alpha (0.4f);
	svg_cairo_render (pSVGContext, m_pCairoContext->cobj ());

	cairo_matrix_init_identity (&matrix);
	cairo_matrix_translate (&matrix, uiWidth * offset.x, uiHeight * offset.y);
	cairo_matrix_rotate (&matrix, (20.0f + get_angle_1 ()) * (M_PI/180.0f));
	cairo_matrix_scale (&matrix, 0.5f, 0.5f);
	m_pCairoContext->set_matrix (matrix);
	m_pCairoContext->paint_with_alpha (0.5f);
	svg_cairo_render (pSVGContext, m_pCairoContext->cobj ());

	svg_cairo_destroy (pSVGContext);

	return true;
}

bool CairoSamples::moving_clip ()
{
	if (!m_pCairoContext)
		return false;

	HandlePoint center = get_center ();
	double fRadius = get_radius () / 2.0f;
	HandlePoint handle = {center.x + fRadius * cos (0.0f),
						  center.y + fRadius * sin (0.0f)};

	draw_background ();

	// draw clipping arc/circle with black interior
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->arc (center.x, center.y, fRadius, 0.0f, 2.0f * M_PI);
	m_pCairoContext->clip ();

	// begin new path within clipping arc/circle
	// current path is not consumed by cairo_clip()
	m_pCairoContext->new_path ();
	m_pCairoContext->rectangle (0.0f, 0.0f, 1.0f, 1.0f);
	m_pCairoContext->fill ();

	// draw two green diagonal lines
	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->move_to (0.0f, 0.0f);
	m_pCairoContext->line_to (1.0f, 1.0f);
	m_pCairoContext->move_to (1.0f, 0.0f);
	m_pCairoContext->line_to (0.0f, 1.0f);
	m_pCairoContext->stroke ();

	// draw the two handle-points */
	draw_handle (&handle);
	draw_handle (&center);

	return true;
}

bool CairoSamples::moving_clip_image ()
{
	if (!m_pCairoContext)
		return false;

	int iWidth;
	int iHeight;
	Cairo::Surface* image;
	HandlePoint center = get_center ();
	double fRadius = get_radius () / 2.0f;
	HandlePoint handle = {center.x + fRadius * cos (0.0f),
						  center.y + fRadius * sin (0.0f)};

	draw_background ();

	// draw clipping arc/circle
	m_pCairoContext->arc (center.x, center.y, fRadius, 0.0f, 2.0f * M_PI);
	m_pCairoContext->clip ();

	// fill that clipping arc/circle with an image
	m_pCairoContext->new_path ();
	//image.create_from_png (m_strImageFileName.c_str ());
	iWidth = image->get_width ();
	iHeight = image->get_height ();

	m_pCairoContext->scale (1.0f / iWidth, 1.0f / iHeight);

	m_pCairoContext->set_source_surface (*image, 0.0f, 0.0f);
	m_pCairoContext->paint ();

	//delete image;

	//m_pCairoContext->scale (iWidth, iHeight);

	// draw the two handle-points
	draw_handle (&handle);
	draw_handle (&center);

	return true;
}

bool CairoSamples::moving_curve_to ()
{
	if (!m_pCairoContext)
		return false;

	HandlePoint points[4];

	points[0] = get_point1 ();
	points[1] = get_point2 ();
	points[2] = get_point3 ();
	points[3] = get_point4 ();

	draw_background ();

	// draw black bezier-curve
	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->move_to (points[0].x, points[0].y);
	m_pCairoContext->curve_to (points[1].x, points[1].y,
							   points[2].x, points[2].y,
							   points[3].x, points[3].y);
	m_pCairoContext->stroke ();

	// draw red "helper" lines
	m_pCairoContext->set_source_rgba (1.0f, 0.2f, 0.2f, 0.6f);
	m_pCairoContext->set_line_width (0.005f);
	m_pCairoContext->move_to (points[0].x, points[0].y);
	m_pCairoContext->line_to (points[1].x, points[1].y);
	m_pCairoContext->move_to (points[2].x, points[2].y);
	m_pCairoContext->line_to (points[3].x, points[3].y);
	m_pCairoContext->stroke ();

	// draw the three handle-points
	draw_handle (&points[0]);
	draw_handle (&points[1]);
	draw_handle (&points[2]);
	draw_handle (&points[3]);

	return true;
}

bool CairoSamples::moving_gradient ()
{
	if (!m_pCairoContext)
		return false;

	HandlePoint center = get_center ();
	double fRadius = get_radius ();
	HandlePoint handle = {center.x + fRadius * cos (0.0f),
						  center.y + fRadius * sin (0.0f)};

	draw_background ();

	Cairo::Pattern pattern = Cairo::Pattern::create_radial (center.x, center.y, fRadius, 0.5f, 0.5f, 0.5f);
	pattern.add_color_stop_rgba (0.0f, 1.0f, 1.0f, 0.1f, 0.85f);
	pattern.add_color_stop_rgba (1.0f, 0.7f, 0.1f, 0.1f, 1.0f);
	m_pCairoContext->set_source (pattern);
	m_pCairoContext->arc (0.5f, 0.5f, 0.3f, 0.0f, 2.0f * M_PI);
	m_pCairoContext->fill ();

	// draw the two handle-points */
	draw_handle (&center);
	draw_handle (&handle);	

	return true;
}

bool CairoSamples::moving_gradient2 ()
{
	if (!m_pCairoContext)
		return false;

	HandlePoint handle1 = get_point1 ();
	HandlePoint handle2 = get_point2 ();

	draw_background ();

	Cairo::Pattern pattern = Cairo::Pattern::create_linear (handle1.x, handle1.y, handle2.x, handle2.y);
	pattern.set_filter (m_cairoFilterMode);
	pattern.set_extend (m_cairoExtendMode);
	pattern.add_color_stop_rgba (0.0f, 1.0f, 1.0f, 0.5f, 1.0f);
	pattern.add_color_stop_rgba (0.1f, 0.3f, 1.0f, 0.8f, 0.5f);
	pattern.add_color_stop_rgba (0.5f, 1.0f, 0.0f, 0.7f, 1.0f);
	pattern.add_color_stop_rgba (0.9f, 0.6f, 0.0f, 1.0f, 0.1f);
	pattern.add_color_stop_rgba (1.0f, 0.2f, 0.5f, 1.0f, 1.0f);
	m_pCairoContext->set_source (pattern);
	m_pCairoContext->rectangle (0.0f, 0.0f, 1.0f, 1.0f);
	m_pCairoContext->fill ();

	// draw the two handle-points */
	draw_handle (&handle1);
	draw_handle (&handle2);	

	return true;
}

bool CairoSamples::moving_zini ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	double fLength = 1.0f / 25.0f;
	double fY;

	m_pCairoContext->set_line_cap (CAIRO_LINE_CAP_ROUND);
	for (int i = 0; i < 60; i++)
	{
		m_pCairoContext->save ();
		m_pCairoContext->set_line_width (fLength);
		m_pCairoContext->translate (0.5f, 0.5f);
		m_pCairoContext->rotate (M_PI/180.0f * (m_ulMilliSeconds + 10.0f*i) * 0.36f);
		fY = 0.33f + 0.0825f * sin ((m_ulMilliSeconds + 10.0f*i)/1000 * 10 * M_PI);
		m_pCairoContext->translate (0, fY);
		m_pCairoContext->rotate (M_PI/180.0f * 6.0f * i);
		m_pCairoContext->set_source_rgba (1.0f, 0.5f, 0.0f, i*0.01f);
		m_pCairoContext->move_to (-fLength, 0);
		m_pCairoContext->line_to (fLength, 0);
		m_pCairoContext->stroke ();
		m_pCairoContext->restore ();
	}

	return true;
}

bool CairoSamples::operator_add ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_ADD);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_atop ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_ATOP);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_atop_reverse ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_DEST_ATOP);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_in ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_IN);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_in_reverse ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_DEST_IN);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_out ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_OUT);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_out_reverse ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_DEST_OUT);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_over ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_OVER);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_over_reverse ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_DEST_OVER);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_saturate ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_SATURATE);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::operator_xor ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	set_bg_svg ("freedesktop.svg");
	m_pCairoContext->set_operator (CAIRO_OPERATOR_XOR);
	m_pCairoContext->set_source_rgba (1.0f, 0.0f, 0.0f, 0.5f);
	m_pCairoContext->rectangle (0.2f, 0.2f, 0.5f, 0.5f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->rectangle (0.4f, 0.4f, 0.4f, 0.4f);
	m_pCairoContext->fill ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->rectangle (0.6f, 0.6f, 0.3f, 0.3f);
	m_pCairoContext->fill ();

	return true;
}

bool CairoSamples::path ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->move_to (0.5f, 0.1f);
	m_pCairoContext->line_to (0.9f, 0.9f);
	m_pCairoContext->rel_line_to (-0.4f, 0.0f);
	m_pCairoContext->curve_to (0.2f, 0.9f, 0.2f, 0.5f, 0.5f, 0.5f);

	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::pattern_fill ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	#define X_FUZZ 0.08
	#define Y_FUZZ 0.08

	#define X_INNER_RADIUS 0.3
	#define Y_INNER_RADIUS 0.2

	#define X_OUTER_RADIUS 0.45
	#define Y_OUTER_RADIUS 0.35

	#define SPIKES 10

	int i;
	double x;
	double y;
	Cairo::TextExtents extents;

	srand (45);
	m_pCairoContext->set_line_width (m_fLineWidth);

	for (i = 0; i < SPIKES * 2; i++)
	{
		x = 0.5f + cos (M_PI * i / SPIKES) * X_INNER_RADIUS +
		(double) rand() * X_FUZZ / RAND_MAX;
		y = 0.5f + sin (M_PI * i / SPIKES) * Y_INNER_RADIUS +
		(double) rand() * Y_FUZZ / RAND_MAX;

		if (i == 0)
			m_pCairoContext->move_to (x, y);
		else
			m_pCairoContext->line_to (x, y);

		i++;

		x = 0.5f + cos (M_PI * i / SPIKES) * X_OUTER_RADIUS +
		(double) rand() * X_FUZZ / RAND_MAX;
		y = 0.5f + sin (M_PI * i / SPIKES) * Y_OUTER_RADIUS +
		(double) rand() * Y_FUZZ / RAND_MAX;

		m_pCairoContext->line_to (x, y);
	}

	m_pCairoContext->close_path ();
	m_pCairoContext->set_source_rgb (1.0f, 0.0f, 0.0f);
	m_pCairoContext->stroke ();

	m_pCairoContext->select_font_face (m_strFontFamily,
									   m_cairoFontSlant,
									   m_cairoFontWeight);

	m_pCairoContext->move_to (x, y);
	m_pCairoContext->text_path (m_strSomeText);

	m_pCairoContext->set_font_size (0.2f);
	m_pCairoContext->text_extents (m_strSomeText, extents);
	x = 0.5f - (extents.width / 2.0f + extents.x_bearing);
	y = 0.5f - (extents.height / 2.0f + extents.y_bearing);

	m_pCairoContext->set_source_rgb (1.0f, 1.0f, 0.5f);
	m_pCairoContext->fill ();

	m_pCairoContext->move_to (x, y);
	m_pCairoContext->text_path (m_strSomeText);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::set_line_cap ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	// draw first (left-most) line, which the user can alter
	m_pCairoContext->set_source_rgb (0.3f, 0.3f, 0.3f);
	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->set_line_cap (m_cairoCapStyle);
	m_pCairoContext->move_to (0.2f, 0.2);
	m_pCairoContext->line_to (0.2f, 0.8);
	m_pCairoContext->stroke ();

	// draw second line using a fixed line-cap style: round
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->set_line_width (0.12f);
	m_pCairoContext->set_line_cap (CAIRO_LINE_CAP_ROUND);
	m_pCairoContext->move_to (0.4f, 0.2);
	m_pCairoContext->line_to (0.4f, 0.8);
	m_pCairoContext->stroke ();

	// draw third line using a fixed line-cap style: square
	m_pCairoContext->set_line_cap (CAIRO_LINE_CAP_SQUARE);
	m_pCairoContext->move_to (0.6f, 0.2);
	m_pCairoContext->line_to (0.6f, 0.8);
	m_pCairoContext->stroke ();

	// draw fourth line using a fixed line-cap style: butt
	m_pCairoContext->set_line_cap (CAIRO_LINE_CAP_BUTT);
	m_pCairoContext->move_to (0.8f, 0.2);
	m_pCairoContext->line_to (0.8f, 0.8);
	m_pCairoContext->stroke ();

	// draw helping lines
	m_pCairoContext->set_line_width (0.005f);
	m_pCairoContext->set_source_rgba (1.0f, 0.2f, 0.2f, 0.6f);
	m_pCairoContext->move_to (0.2f, 0.2f);
	m_pCairoContext->line_to (0.2f, 0.8f);
	m_pCairoContext->move_to (0.4f, 0.2f);
	m_pCairoContext->line_to (0.4f, 0.8f);
	m_pCairoContext->move_to (0.6f, 0.2f);
	m_pCairoContext->line_to (0.6f, 0.8f);
	m_pCairoContext->move_to (0.8f, 0.2f);
	m_pCairoContext->line_to (0.8f, 0.8f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::set_line_join ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	// draw first (top-left) joint
	m_pCairoContext->set_source_rgb (0.3f, 0.3f, 0.3f);
	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->move_to (0.125f, 0.475f);
	m_pCairoContext->rel_line_to (0.125f, -0.25f);
	m_pCairoContext->rel_line_to (0.125f, 0.25f);
	m_pCairoContext->set_line_join (m_cairoJoinStyle);
	m_pCairoContext->stroke ();

	// draw second (top-right) joint using line-join style: miter
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->set_line_width (0.16f);
	m_pCairoContext->move_to (0.625f, 0.475f);
	m_pCairoContext->rel_line_to (0.125f, -0.25f);
	m_pCairoContext->rel_line_to (0.125f, 0.25f);
	m_pCairoContext->set_line_join (CAIRO_LINE_JOIN_MITER);
	m_pCairoContext->stroke ();

	// draw third (bottom-left) joint using line-join style: bevel
	m_pCairoContext->move_to (0.125f, 0.925f);
	m_pCairoContext->rel_line_to (0.125f, -0.25f);
	m_pCairoContext->rel_line_to (0.125f, 0.25f);
	m_pCairoContext->set_line_join (CAIRO_LINE_JOIN_BEVEL);
	m_pCairoContext->stroke ();

	// draw fourth (bottom-right) joint using line-join style: round
	m_pCairoContext->move_to (0.625f, 0.925f);
	m_pCairoContext->rel_line_to (0.125f, -0.25f);
	m_pCairoContext->rel_line_to (0.125f, 0.25f);
	m_pCairoContext->set_line_join (CAIRO_LINE_JOIN_ROUND);
	m_pCairoContext->stroke ();

	// draw helping line (top-left)
	m_pCairoContext->set_line_width (0.005f);
	m_pCairoContext->set_source_rgba (1.0f, 0.2f, 0.2f, 0.6f);
	m_pCairoContext->move_to (0.125f, 0.475f);
	m_pCairoContext->rel_line_to (0.125f, -0.25f);
	m_pCairoContext->rel_line_to (0.125f, 0.25f);

	// draw helping line (top-right)
	m_pCairoContext->move_to (0.625f, 0.475f);
	m_pCairoContext->rel_line_to (0.125f, -0.25f);
	m_pCairoContext->rel_line_to (0.125f, 0.25f);

	// draw helping line (bottom-left)
	m_pCairoContext->move_to (0.125f, 0.925f);
	m_pCairoContext->rel_line_to (0.125f, -0.25f);
	m_pCairoContext->rel_line_to (0.125f, 0.25f);

	// draw helping line (bottom-right)
	m_pCairoContext->move_to (0.625f, 0.925f);
	m_pCairoContext->rel_line_to (0.125f, -0.25f);
	m_pCairoContext->rel_line_to (0.125f, 0.25f);

	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::text_align_center ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	// set font-attributes
	m_pCairoContext->select_font_face (m_strSomeText,
									   m_cairoFontSlant,
									   m_cairoFontWeight);
	m_pCairoContext->set_font_size (m_fFontSize);

	// get texts extents
	Cairo::TextExtents extents;
	m_pCairoContext->text_extents (m_strSomeText, extents);

	// determine center point
	double fX;
	double fY;
	fX = 0.5f - (extents.width / 2.0f + extents.x_bearing);
	fY = 0.5f - (extents.height / 2.0f + extents.y_bearing);

	// draw the solid black text
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->move_to (fX, fY);
	m_pCairoContext->show_text (m_strSomeText.c_str ());

	// draw helping lines
	m_pCairoContext->set_line_width (0.005f);
	m_pCairoContext->set_source_rgba (1.0f, 0.2f, 0.2f, 0.6f);
	m_pCairoContext->move_to (0.5f, 0.0f);
	m_pCairoContext->rel_line_to (0.0f, 1.0f);
	m_pCairoContext->move_to (0.0f, 0.5f);
	m_pCairoContext->rel_line_to (1.0f, 0.0f);
	m_pCairoContext->stroke ();

	// draw handle
	draw_handle (fX, fY);

	return true;
}

bool CairoSamples::text ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	// set font-attributes
	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->select_font_face (m_strFontFamily,
									   m_cairoFontSlant,
									   m_cairoFontWeight);
	m_pCairoContext->set_font_size (m_fFontSize);

	// draw the solid black text
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->move_to (0.04f, 0.53f);
	m_pCairoContext->show_text (m_strSomeText);

	// draw the blue colored text with black outlines
	m_pCairoContext->move_to (0.27f, 0.65f);
	m_pCairoContext->text_path (m_strSomeText);
	m_pCairoContext->set_source_rgb (0.5f, 0.5f, 1.0f);
	m_pCairoContext->fill_preserve ();
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->stroke ();

	// draw the two handle-points
	draw_handle (0.04f, 0.53f);
	draw_handle (0.27f, 0.65f);

	return true;
}

bool CairoSamples::text_extents ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	// set font-attributes
	m_pCairoContext->select_font_face (m_strFontFamily.c_str (),
									   m_cairoFontSlant,
									   m_cairoFontWeight);
	m_pCairoContext->set_font_size (m_fFontSize);

	// get texts extents
	Cairo::TextExtents extents;
	m_pCairoContext->text_extents (m_strSomeText.c_str (), extents);

	double fX = 0.1f;
	double fY = 0.6f;

	// draw the solid black text
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->move_to (fX, fY);
	m_pCairoContext->show_text (m_strSomeText.c_str ());

	// draw helping lines
	m_pCairoContext->set_line_width (0.005f);
	m_pCairoContext->set_source_rgba (1.0f, 0.2f, 0.2f, 0.6f);
	m_pCairoContext->move_to (fX, fY);
	m_pCairoContext->rel_line_to (0.0f, -extents.height);
	m_pCairoContext->rel_line_to (extents.width, 0.0f);
	m_pCairoContext->rel_line_to (extents.x_bearing, -extents.y_bearing);
	m_pCairoContext->stroke ();

	// draw the handle-point
	draw_handle (fX, fY);

	return true;
}

bool CairoSamples::xxx_clip_rectangle ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->new_path ();
	m_pCairoContext->move_to (.25f, .25f);
	m_pCairoContext->line_to (.25f, .75f);
	m_pCairoContext->line_to (.75f, .75f);
	m_pCairoContext->line_to (.75f, .25f);
	m_pCairoContext->line_to (.25f, .25f);
	m_pCairoContext->close_path ();

	m_pCairoContext->clip ();

	m_pCairoContext->move_to (0.0f, 0.0f);
	m_pCairoContext->line_to (1.0f, 1.0f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::xxx_dash ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	double offset = -0.2f;
	std::valarray<double> dashes (4);
	dashes[0] = 0.20f; /* ink */
    dashes[1] = 0.05f; /* skip */
    dashes[2] = 0.05f; /* ink */
    dashes[3] = 0.05f; /* skip */

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->set_line_cap (m_cairoCapStyle);
	m_pCairoContext->set_line_join (m_cairoJoinStyle);
	m_pCairoContext->set_dash (dashes, offset);

	m_pCairoContext->move_to (0.5f, 0.1f);
	m_pCairoContext->line_to (0.9f, 0.9f);
	m_pCairoContext->rel_line_to (-0.4f, 0.0f);
	m_pCairoContext->curve_to (0.2f, 0.9f, 0.2f, 0.5f, 0.5f, 0.5f);

	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::xxx_long_lines ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->move_to (0.1f, -50.0f);
	m_pCairoContext->line_to (0.1f,  50.0f);
	m_pCairoContext->set_source_rgb (1.0f, 0.0f, 0.0f);
	m_pCairoContext->stroke ();

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->move_to (0.2f, -60.0f);
	m_pCairoContext->line_to (0.2f,  60.0f);
	m_pCairoContext->set_source_rgb (1.0f, 1.0f, 0.0f);
	m_pCairoContext->stroke ();

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->move_to (0.3f, -70.0f);
	m_pCairoContext->line_to (0.3f,  70.0f);
	m_pCairoContext->set_source_rgb (0.0f, 1.0f, 0.0f);
	m_pCairoContext->stroke ();

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->move_to (0.4f, -80.0f);
	m_pCairoContext->line_to (0.4f,  80.0f);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 1.0f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::xxx_multi_segment_caps ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	m_pCairoContext->move_to (0.2f, 0.3f);
	m_pCairoContext->line_to (0.8f, 0.3f);

	m_pCairoContext->move_to (0.2f, 0.5f);
	m_pCairoContext->line_to (0.8f, 0.5f);

	m_pCairoContext->move_to (0.2f, 0.7f);
	m_pCairoContext->line_to (0.8f, 0.7f);

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->set_line_cap (m_cairoCapStyle);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->stroke ();

	return true;
}

bool CairoSamples::xxx_self_intersect ()
{
	if (!m_pCairoContext)
		return false;

	draw_background ();

	m_pCairoContext->move_to (0.3f, 0.3f);
	m_pCairoContext->line_to (0.7f, 0.3f);

	m_pCairoContext->line_to (0.5f, 0.3f);
	m_pCairoContext->line_to (0.5f, 0.7f);

	m_pCairoContext->set_line_width (m_fLineWidth);
	m_pCairoContext->set_line_cap (m_cairoCapStyle);
	m_pCairoContext->set_line_join (m_cairoJoinStyle);
	m_pCairoContext->set_source_rgb (0.0f, 0.0f, 0.0f);
	m_pCairoContext->stroke ();
       
	return true;
}
