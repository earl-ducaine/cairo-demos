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

#ifndef _CAIROSAMPLES_H_
#define _CAIROSAMPLES_H_

#include <cairomm/cairomm.h>
#include <string>
#include <gdkmm.h>

#include "sample-list-store.h"

typedef struct _HandlePoint
{
	double x;
	double y;
} HandlePoint;

class CairoSamples
{
	public:
		CairoSamples ();
		~CairoSamples ();

		typedef struct _Entry
		{
			std::string	name;
		} Entry;

		enum Sample
		{
			CurveRectangle = 0,
			Clock,
			FillAndStroke,
			FillAndStroke2,
			Fontmania,
			Gradient,
			Image,
			ImagePattern,
			LibSVG,
			MovingArc,
			MovingArcNegative,
			MovingCairoLogo,
			MovingClip,
			MovingClipImage,
			MovingCurveTo,
			MovingGradient,
			MovingGradient2,
			MovingZini,
			OperatorAdd,
			OperatorAtop,
			OperatorAtopReverse,
			OperatorIn,
			OperatorInReverse,
			OperatorOut,
			OperatorOutReverse,
			OperatorOver,
			OperatorOverReverse,
			OperatorSaturate,
			OperatorXor,
			Path,
			PatternFill,
			SetLineCap,
			SetLineJoin,
			TextAlignCenter,
			Text,
			TextExtents,
			XxxClipRectangle,
			XxxDash,
			XxxLongLines,
			XxxMultiSegmentCaps,
			XxxSelfIntersect,
			NumberOfSamples
		};

		enum DashStyle
		{
			DashOff = 0,
			Dotted,
			Dashed,
			DashDotDash,
			DashedFunky
		};

		void do_animation_step (unsigned long ulMilliSeconds);
		void set_context (Cairo::Context* pCairoContext);
		void normalize_canvas (double fWidth, double fHeight) const;
		void set_bg_svg (std::string strSVGFileName);
		bool do_sample (Sample sample);
		void set_cap_style (Cairo::LineCap cairoCapStyle);
		void set_join_style (Cairo::LineJoin cairoJoinStyle);
		void set_dash_style (DashStyle dashStyle);
		void set_operator (Cairo::Operator cairoOperator);
		void set_extend_mode (Cairo::Extend cairoExtendMode);
		void set_filter_mode (Cairo::Filter cairoFilterMode);
		void set_line_width (double fLineWidth);
		void set_bg_color (Gdk::Color bgColor);
		void set_font_family (std::string strFontFamily);
		void set_font_size (double fFontSize);
		void set_font_weight (Cairo::FontWeight cairoFontWeight);
		void set_font_slant (Cairo::FontSlant cairoFontSlant);
		void set_some_text (std::string strSomeText);
		void set_image_file_name (std::string strFileName);
		void set_svg_file_name (std::string strFileName);
		void add_samples_to_list_store (SampleListStore* pSampleListStore);
		std::string get_sample_name (Sample sample);

	private:
		void update_var (double* pfValue,
						 double fLowerLimit,
						 double fUpperLimit,
						 double* pfStep,
						 double fGrow,
						 double fShrink);
		HandlePoint get_center ();
		HandlePoint get_point1 ();
		HandlePoint get_point2 ();
		HandlePoint get_point3 ();
		HandlePoint get_point4 ();
		double get_radius ();
		double get_angle_1 ();
		double get_angle_2 ();
		void draw_background ();
		void draw_background (Gdk::Color color);
		void draw_handle (double fX, double fY);
		void draw_handle (HandlePoint* pPoint);
		bool sample_fallback (std::string strMessage);
		bool curve_rectangle ();
		bool clock ();
		bool fill_and_stroke ();
		bool fill_and_stroke2 ();
		bool fontmania ();
		bool gradient ();
		bool image ();
		bool image_pattern ();
		bool libsvg ();
		bool moving_arc ();
		bool moving_arc_negative ();
		bool moving_cairo_logo ();
		bool moving_clip ();
		bool moving_clip_image ();
		bool moving_curve_to ();
		bool moving_gradient ();
		bool moving_gradient2 ();
		bool moving_zini ();
		bool operator_add ();
		bool operator_atop ();
		bool operator_atop_reverse ();
		bool operator_in ();
		bool operator_in_reverse ();
		bool operator_out ();
		bool operator_out_reverse ();
		bool operator_over ();
		bool operator_over_reverse ();
		bool operator_saturate ();
		bool operator_xor ();
		bool path ();
		bool pattern_fill ();
		bool set_line_cap ();
		bool set_line_join ();
		bool text_align_center ();
		bool text ();
		bool text_extents ();
		bool xxx_clip_rectangle ();
		bool xxx_dash ();
		bool xxx_long_lines ();
		bool xxx_multi_segment_caps ();
		bool xxx_self_intersect ();

	protected:
		HandlePoint			m_center;
		HandlePoint			m_point1;
		HandlePoint			m_point2;
		HandlePoint			m_point3;
		HandlePoint			m_point4;
		double				m_fRadius;
		double				m_fAngle1;
		double				m_fAngle2;
		double				m_centerXStep;
		double				m_centerYStep;
		double				m_point1XStep;
		double				m_point1YStep;
		double				m_point2XStep;
		double				m_point2YStep;
		double				m_point3XStep;
		double				m_point3YStep;
		double				m_point4XStep;
		double				m_point4YStep;
		double				m_fRadiusStep;
		double				m_fAngle1Step;
		double				m_fAngle2Step;
		Cairo::Context*		m_pCairoContext;
		Cairo::LineCap		m_cairoCapStyle;
		Cairo::LineJoin		m_cairoJoinStyle;
		DashStyle			m_dashStyle;
		Cairo::Operator		m_cairoOperator;
		Cairo::Extend		m_cairoExtendMode;
		Cairo::Filter		m_cairoFilterMode;
		double				m_fLineWidth;
		Gdk::Color			m_bgColor;
		std::string			m_strFontFamily;
		double				m_fFontSize;
		Cairo::FontWeight	m_cairoFontWeight;
		Cairo::FontSlant	m_cairoFontSlant;
		std::string			m_strSomeText;
		std::string			m_strImageFileName;
		std::string			m_strSvgFileName;
		std::vector<Entry>	m_sampleList;
		unsigned long		m_ulMilliSeconds;
};

#endif /*_CAIROSAMPLES_H_*/
