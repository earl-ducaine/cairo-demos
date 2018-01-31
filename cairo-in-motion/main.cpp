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

#include <libglademm/xml.h>
#include <gtkmm.h>
#include <iostream>
#include <sstream>
#include <math.h>

#include "cairo-samples.h"
#include "sample-list-store.h"

// widgets we get from the .glade file
Gtk::Window* pMainWindow = NULL;
Gtk::Dialog* pAboutDialog = NULL;
Gtk::Statusbar* pMainStatusBar = NULL;
Gtk::ComboBox* pCapStyleComboBox = NULL;
Gtk::ComboBox* pJoinStyleComboBox = NULL;
Gtk::ComboBox* pDashStyleComboBox = NULL;
Gtk::ComboBox* pOperatorComboBox = NULL;
Gtk::ComboBox* pExtendModeComboBox = NULL;
Gtk::ComboBox* pFilterModeComboBox = NULL;
Gtk::HScale* pLineWidthHScale = NULL;
Gtk::ColorButton* pBGColorColorButton = NULL;
Gtk::ToggleButton* pAnimateToggleButton = NULL;
Gtk::FontButton* pFontFontButton = NULL;
Gtk::TreeView* pSamplesTreeView = NULL;
Gtk::DrawingArea* pMainDrawingArea = NULL;
Gtk::Entry* pTextEntry = NULL;
Gtk::FileChooserButton* pImageFileFileChooserButton = NULL;
Gtk::FileChooserButton* pSvgFileFileChooserButton = NULL;
Gtk::HScale* pAnimSmoothnessHScale = NULL;

// those here are just used to give some proper default values to everything
// later during runtime most of those here aren't touch anymore... rather the
// values are directly passed to the class CairoSamples
Cairo::LineCap m_capStyle = CAIRO_LINE_CAP_ROUND;
Cairo::LineJoin m_joinStyle = CAIRO_LINE_JOIN_ROUND;
CairoSamples::DashStyle m_dashStyle = CairoSamples::DashOff;
Cairo::Operator m_operator = CAIRO_OPERATOR_CLEAR;
Cairo::Extend m_extendMode = CAIRO_EXTEND_REPEAT;
Cairo::Filter m_filterMode = CAIRO_FILTER_GOOD;
double m_fLineWidth = 0.01f;
Gdk::Color m_bgColor ("#B0B0B0");
std::string m_strFontFamily = "Sans";
int m_iFontSize = 35;
Cairo::FontWeight m_fontWeight = CAIRO_FONT_WEIGHT_NORMAL;
Cairo::FontSlant m_fontSlant = CAIRO_FONT_SLANT_NORMAL;
std::string m_strImageFileName = "/home/mirco/workspace/cairo-in-motion/gotroot.png";
std::string m_strSvgFileName = "/home/mirco/workspace/cairo-in-motion/freedesktop.svg";
std::string m_strSomeText = "laber";

// additional stuff we need/use for the cairo-drawing
Gdk::Window* m_pGdkWindow = NULL;
bool m_bAnimate = true;
Cairo::Context* m_pCairoContext = NULL;
CairoSamples* m_pCairoSamples = NULL;
SampleListStore* m_pSampleListStore = NULL;
int m_iSample = 17;
int m_iAnimSmoothness = 25;
sigc::connection timeoutHandlerConnection;
Glib::Timer timer;

void on_quit_menu_item ()
{
	pMainStatusBar->push ("quit requested");

	if (pMainWindow)
		pMainWindow->hide (); //hide() will cause main::run() to end.
	else
		std::cout << "ehm... no valid pointer to main-window" << std::endl;
}

void on_info_menu_item ()
{
	pMainStatusBar->push ("info requested");

	if (pAboutDialog)
		pAboutDialog->show ();
	else
		std::cout << "ehm... no valid pointer to about-dialog" << std::endl;
}

void on_cap_style_changed ()
{
	switch (pCapStyleComboBox->get_active_row_number ())
	{
		case CAIRO_LINE_CAP_BUTT :
			m_pCairoSamples->set_cap_style (CAIRO_LINE_CAP_BUTT);
			pMainStatusBar->push ("cap-style changed to CAIRO_LINE_CAP_BUTT");
		break;

		case CAIRO_LINE_CAP_ROUND :
			m_pCairoSamples->set_cap_style (CAIRO_LINE_CAP_ROUND);
			pMainStatusBar->push ("cap-style changed to CAIRO_LINE_CAP_ROUND");
		break;

		case CAIRO_LINE_CAP_SQUARE :
			m_pCairoSamples->set_cap_style (CAIRO_LINE_CAP_SQUARE);
			pMainStatusBar->push ("cap-style changed to CAIRO_LINE_CAP_SQUARE");
		break;
	}

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_join_style_changed ()
{
	switch (pJoinStyleComboBox->get_active_row_number ())
	{
		case CAIRO_LINE_JOIN_MITER :
			m_pCairoSamples->set_join_style (CAIRO_LINE_JOIN_MITER);
			pMainStatusBar->push ("join-style changed to CAIRO_LINE_JOIN_MITER");
		break;

		case CAIRO_LINE_JOIN_ROUND :
			m_pCairoSamples->set_join_style (CAIRO_LINE_JOIN_ROUND);
			pMainStatusBar->push ("join-style changed to CAIRO_LINE_JOIN_ROUND");
		break;

		case CAIRO_LINE_JOIN_BEVEL :
			m_pCairoSamples->set_join_style (CAIRO_LINE_JOIN_BEVEL);
			pMainStatusBar->push ("join-style changed to CAIRO_LINE_JOIN_BEVEL");
		break;
	}

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_dash_style_changed ()
{
	switch (pDashStyleComboBox->get_active_row_number ())
	{
		case CairoSamples::DashOff :
			m_pCairoSamples->set_dash_style (CairoSamples::DashOff);
			pMainStatusBar->push ("dash-style changed to DashOff");
		break;

		case CairoSamples::Dotted :
			m_pCairoSamples->set_dash_style (CairoSamples::Dotted);
			pMainStatusBar->push ("dash-style changed to Dotted");
		break;

		case CairoSamples::Dashed :
			m_pCairoSamples->set_dash_style (CairoSamples::Dashed);
			pMainStatusBar->push ("dash-style changed to Dashed");
		break;

		case CairoSamples::DashDotDash :
			m_pCairoSamples->set_dash_style (CairoSamples::DashDotDash);
			pMainStatusBar->push ("dash-style changed to DashDotDash");
		break;

		case CairoSamples::DashedFunky :
			m_pCairoSamples->set_dash_style (CairoSamples::DashedFunky);
			pMainStatusBar->push ("dash-style changed to DashedFunky");
		break;
	}

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_operator_changed ()
{
	switch (pOperatorComboBox->get_active_row_number ())
	{
		case CAIRO_OPERATOR_CLEAR :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_CLEAR);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_CLEAR");
		break;

		case CAIRO_OPERATOR_SOURCE :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_SOURCE);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_SOURCE");
		break;

		case CAIRO_OPERATOR_OVER :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_OVER);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_OVER");
		break;

		case CAIRO_OPERATOR_IN :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_IN);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_IN");
		break;

		case CAIRO_OPERATOR_OUT :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_OUT);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_OUT");
		break;

		case CAIRO_OPERATOR_ATOP :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_ATOP);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_ATOP");
		break;

		case CAIRO_OPERATOR_DEST :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_DEST);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_DEST");
		break;

		case CAIRO_OPERATOR_DEST_OVER :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_DEST_OVER);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_DEST_OVER");
		break;

		case CAIRO_OPERATOR_DEST_IN :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_DEST_IN);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_DEST_IN");
		break;

		case CAIRO_OPERATOR_DEST_OUT :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_DEST_OUT);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_DEST_OUT");
		break;

		case CAIRO_OPERATOR_DEST_ATOP :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_DEST_ATOP);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_DEST_ATOP");
		break;

		case CAIRO_OPERATOR_XOR :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_XOR);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_XOR");
		break;

		case CAIRO_OPERATOR_ADD :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_ADD);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_ADD");
		break;

		case CAIRO_OPERATOR_SATURATE :
			m_pCairoSamples->set_operator (CAIRO_OPERATOR_SATURATE);
			pMainStatusBar->push ("operator changed to CAIRO_OPERATOR_SATURATE");
		break;
	}

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}	

void on_extend_mode_changed ()
{
	switch (pExtendModeComboBox->get_active_row_number ())
	{
		case CAIRO_EXTEND_NONE :
			m_pCairoSamples->set_extend_mode (CAIRO_EXTEND_NONE);
			pMainStatusBar->push ("extend-mode changed to CAIRO_EXTEND_NONE");
		break;

		case CAIRO_EXTEND_REPEAT :
			m_pCairoSamples->set_extend_mode (CAIRO_EXTEND_REPEAT);
			pMainStatusBar->push ("extend-mode changed to CAIRO_EXTEND_REPEAT");
		break;

		case CAIRO_EXTEND_REFLECT :
			m_pCairoSamples->set_extend_mode (CAIRO_EXTEND_REFLECT);
			pMainStatusBar->push ("extend-mode changed to CAIRO_EXTEND_REFLECT");
		break;
	}

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_filter_mode_changed ()
{
	switch (pFilterModeComboBox->get_active_row_number ())
	{
		case CAIRO_FILTER_FAST :
			m_pCairoSamples->set_filter_mode (CAIRO_FILTER_FAST);
			pMainStatusBar->push ("filter-mode changed to CAIRO_FILTER_FAST");
		break;

		case CAIRO_FILTER_GOOD :
			m_pCairoSamples->set_filter_mode (CAIRO_FILTER_GOOD);
			pMainStatusBar->push ("filter-mode changed to CAIRO_FILTER_GOOD");
		break;

		case CAIRO_FILTER_BEST :
			m_pCairoSamples->set_filter_mode (CAIRO_FILTER_BEST);
			pMainStatusBar->push ("filter-mode changed to CAIRO_FILTER_BEST");
		break;

		case CAIRO_FILTER_NEAREST :
			m_pCairoSamples->set_filter_mode (CAIRO_FILTER_NEAREST);
			pMainStatusBar->push ("filter-mode changed to CAIRO_FILTER_NEAREST");
		break;

		case CAIRO_FILTER_BILINEAR :
			m_pCairoSamples->set_filter_mode (CAIRO_FILTER_BILINEAR);
			pMainStatusBar->push ("filter-mode changed to CAIRO_FILTER_BILINEAR");
		break;

		case CAIRO_FILTER_GAUSSIAN :
			m_pCairoSamples->set_filter_mode (CAIRO_FILTER_GAUSSIAN);
			pMainStatusBar->push ("filter-mode changed to CAIRO_FILTER_GAUSSIAN");
		break;
	}

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_line_width_changed ()
{
	m_pCairoSamples->set_line_width ((double) pLineWidthHScale->get_value ());

	std::stringstream message;
	message << "line-width changed to " << pLineWidthHScale->get_value ();
	pMainStatusBar->push (message.str ());

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_bg_color_set ()
{
	m_pCairoSamples->set_bg_color (pBGColorColorButton->get_color ());

	std::stringstream message;
	message << "background-color changed to "
			<< pBGColorColorButton->get_color().get_red_p ()
			<< "/"
			<< pBGColorColorButton->get_color().get_green_p ()
			<< "/"
			<< pBGColorColorButton->get_color().get_blue_p ();
	pMainStatusBar->push (message.str ());

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

bool on_timeout ()
{
	static unsigned long ulMicroSeconds;

	timer.elapsed (ulMicroSeconds);
	m_pCairoSamples->do_animation_step (ulMicroSeconds/1000);

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();

	return true;
}

void on_anim_smoothness_changed ()
{
	m_iAnimSmoothness = (int) pAnimSmoothnessHScale->get_value ();

	std::stringstream message;
	message << "animation smoothness changed to " << pAnimSmoothnessHScale->get_value ();
	pMainStatusBar->push (message.str ());

	if (m_bAnimate)
	{
		timeoutHandlerConnection.disconnect ();
		timeoutHandlerConnection = Glib::signal_timeout().connect (sigc::ptr_fun(&on_timeout), m_iAnimSmoothness);
	}

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_animate_toggled ()
{
	if (pAnimateToggleButton->get_active ())
	{
		timeoutHandlerConnection = Glib::signal_timeout().connect (sigc::ptr_fun(&on_timeout), m_iAnimSmoothness);
		m_bAnimate = true;
		pMainStatusBar->push ("animation is turned on");
	}
	else
	{
		timeoutHandlerConnection.disconnect ();
		m_bAnimate = false;
		pMainStatusBar->push ("animation is turned off");
	}
}

void on_sample_activated (const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
	// this feels awkward/wrong, isn't there another way to get the
	// integer-values form a Gtk::TreeModel::Path ?
	std::stringstream str2int;
	str2int << path.to_string ();
	str2int >> m_iSample;

	std::stringstream message;
	message << "rendering sample \""
			<< m_pCairoSamples->get_sample_name ((CairoSamples::Sample) m_iSample)
			<< "\"";

	pMainStatusBar->push (message.str ());

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

bool on_expose_event (GdkEventExpose* pEvent)
{
	m_pCairoContext = new Cairo::Context (gdk_cairo_create (pMainDrawingArea->get_window()->gobj ()));
	if (m_pCairoContext)
	{
		m_pCairoSamples->set_context (m_pCairoContext);
		m_pCairoSamples->normalize_canvas ((double) pMainDrawingArea->get_allocation().get_width (),
										   (double) pMainDrawingArea->get_allocation().get_height ());
		m_pCairoSamples->do_sample ((CairoSamples::Sample) m_iSample);
		delete m_pCairoContext;
	}

	return true;
}

// this doesn't catch every slant- and weight-attribute-names there are, but it
// covers most of the usual "Regular", "Bold", "Italic", "Oblique" bits... this
// is only a testbed after all and no full-blown application that needs to be
// bullet-proof... aside from that, I'm always happy to receive patches :)
void on_font_set ()
{
	std::string strFontName = pFontFontButton->get_font_name ();
	std::string strToken;
	std::string::size_type start;
	std::string::size_type length;

	std::stringstream message;
	message << "font changed to "
			<< strFontName;
	pMainStatusBar->push (message.str ());

	// set default slant and weight
	m_pCairoSamples->set_font_slant (CAIRO_FONT_SLANT_NORMAL);
	m_pCairoSamples->set_font_weight (CAIRO_FONT_WEIGHT_NORMAL);

	// just find one occurance of "Regular" and eat it
	strToken = "Regular";
	start = strFontName.find (strToken);
	length = strToken.length ();
	if (start != std::string::npos)
		strFontName.erase (start, length);

	// find one occurance of "Roman" and eat it 
	// *sigh* this bites with "Times New Roman"
	strToken = "Roman";
	start = strFontName.find (strToken);
	length = strToken.length ();
	if (start != std::string::npos)
		strFontName.erase (start, length);

	// find one occurance of "Bold"
	strToken = "Bold";
	start = strFontName.find (strToken);
	length = strToken.length ();
	if (start != std::string::npos)
	{
		m_pCairoSamples->set_font_weight (CAIRO_FONT_WEIGHT_BOLD);
		strFontName.erase (start, length);
	}

	// find one occurance of "Italic"
	strToken = "Italic";
	start = strFontName.find (strToken);
	length = strToken.length ();
	if (start != std::string::npos)
	{
		m_pCairoSamples->set_font_slant (CAIRO_FONT_SLANT_ITALIC);
		strFontName.erase (start, length);
	}

	// find one occurance of "Oblique"
	strToken = "Oblique";
	start = strFontName.find (strToken);
	length = strToken.length ();
	if (start != std::string::npos)
	{
		m_pCairoSamples->set_font_slant (CAIRO_FONT_SLANT_OBLIQUE);
		strFontName.erase (start, length);
	}

	// just find and cut one occurance of "semicondensed"
	strToken = "semicondensed";
	start = strFontName.find (strToken);
	length = strToken.length ();
	if (start != std::string::npos)
		strFontName.erase (start, length);

	// get font-size
	start = strFontName.find_last_of (" ");
	std::stringstream str2int;
	str2int << strFontName.substr (start);
	str2int >> m_iFontSize;
	m_pCairoSamples->set_font_size (m_iFontSize * 0.01f);

	// cut size from remaining string
	start = strFontName.find_last_of (" ");
	if (start != std::string::npos)
		strFontName.erase (start);

	// cut trailing spaces from remaining string
	start = strFontName.find_last_of (" ");
	if (start != std::string::npos)
		strFontName.erase (start);

	// cut trailing comma from remaining string
	start = strFontName.find_last_of (",");
	if (start != std::string::npos)
		strFontName.erase (start);

	// finally the remaining string only contains the font-family name
	m_pCairoSamples->set_font_family (strFontName);

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_text_entry_activated ()
{
	std::stringstream message;
	message << "some text changed to "
			<< "\""
			<< pTextEntry->get_text ()
			<< "\"";
	pMainStatusBar->push (message.str ());

	m_pCairoSamples->set_some_text ((std::string) pTextEntry->get_text ());

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_image_selection_changed ()
{
	std::stringstream message;
	message << "PNG-file changed to "
			<< "\""
			<< pImageFileFileChooserButton->get_filename ()
			<< "\"";
	pMainStatusBar->push (message.str ());

	m_pCairoSamples->set_image_file_name ((std::string) pImageFileFileChooserButton->get_filename ());

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

void on_svg_selection_changed ()
{
	std::stringstream message;
	message << "SVG-file changed to "
			<< "\""
			<< pSvgFileFileChooserButton->get_filename ()
			<< "\"";
	pMainStatusBar->push (message.str ());

	m_pCairoSamples->set_svg_file_name ((std::string) pSvgFileFileChooserButton->get_filename ());

	// force a redraw of the cairo-context
	pMainDrawingArea->queue_draw ();
}

int main (int argc, char** argv)
{
	Gtk::Main app (argc, argv);

	Glib::RefPtr<Gnome::Glade::Xml> refXml;
	try
	{
		refXml = Gnome::Glade::Xml::create("cairo-in-motion.glade");
	}
	catch (const Gnome::Glade::XmlError& exception)
	{
		std::cerr << exception.what () << std::endl;
		return 1;
	}

	refXml->get_widget ("mainWindow", pMainWindow);
	refXml->get_widget ("aboutDialog", pAboutDialog);
	if (pMainWindow)
	{
		Gtk::ImageMenuItem* pQuitMenuItem = NULL;
		Gtk::MenuItem* pInfoMenuItem = NULL;
		refXml->get_widget ("quitMenuItem", pQuitMenuItem);
		refXml->get_widget ("infoMenuItem", pInfoMenuItem);
		refXml->get_widget ("mainStatusBar", pMainStatusBar);
		if (pQuitMenuItem)
			pQuitMenuItem->signal_activate().connect (sigc::ptr_fun (on_quit_menu_item));
		if (pInfoMenuItem)
			pInfoMenuItem->signal_activate().connect (sigc::ptr_fun (on_info_menu_item));
		if (pMainStatusBar)
			pMainStatusBar->push ("program started");

		pMainWindow->set_icon_from_file ("cairo-in-motion.png");
		pAboutDialog->set_icon_from_file ("cairo-in-motion.png");

		refXml->get_widget ("capStyleComboBox", pCapStyleComboBox);
		refXml->get_widget ("joinStyleComboBox", pJoinStyleComboBox);
		refXml->get_widget ("dashStyleComboBox", pDashStyleComboBox);
		refXml->get_widget ("operatorComboBox", pOperatorComboBox);
		refXml->get_widget ("extendModeComboBox", pExtendModeComboBox);
		refXml->get_widget ("filterModeComboBox", pFilterModeComboBox);
		if (pCapStyleComboBox)
		{
			pCapStyleComboBox->set_active ((int) m_capStyle);
			pCapStyleComboBox->signal_changed().connect (sigc::ptr_fun (on_cap_style_changed));
		}
		if (pJoinStyleComboBox)
		{
			pJoinStyleComboBox->set_active ((int) m_joinStyle);
			pJoinStyleComboBox->signal_changed().connect (sigc::ptr_fun (on_join_style_changed));
		}
		if (pDashStyleComboBox)
		{
			pDashStyleComboBox->set_active ((int) m_dashStyle);
			pDashStyleComboBox->signal_changed().connect (sigc::ptr_fun (on_dash_style_changed));
		}
		if (pOperatorComboBox)
		{
			pOperatorComboBox->set_active ((int) m_operator);
			pOperatorComboBox->signal_changed().connect (sigc::ptr_fun (on_operator_changed));
		}
		if (pExtendModeComboBox)
		{
			pExtendModeComboBox->set_active ((int) m_extendMode);
			pExtendModeComboBox->signal_changed().connect (sigc::ptr_fun (on_extend_mode_changed));
		}
		if (pFilterModeComboBox)
		{
			pFilterModeComboBox->set_active ((int) m_filterMode);
			pFilterModeComboBox->signal_changed().connect (sigc::ptr_fun (on_filter_mode_changed));
		}

		refXml->get_widget ("lineWidthHScale", pLineWidthHScale);
		if (pLineWidthHScale)
		{
			pLineWidthHScale->set_value (m_fLineWidth);
			pLineWidthHScale->signal_value_changed().connect (sigc::ptr_fun (on_line_width_changed));
		}

		refXml->get_widget ("bgColorColorButton", pBGColorColorButton);
		if (pBGColorColorButton)
		{
			pBGColorColorButton->set_color (m_bgColor);
			pBGColorColorButton->signal_color_set().connect (sigc::ptr_fun (on_bg_color_set));
		}

		refXml->get_widget ("animateToggleButton", pAnimateToggleButton);
		if (pAnimateToggleButton)
		{
			timeoutHandlerConnection = Glib::signal_timeout().connect(sigc::ptr_fun (&on_timeout), m_iAnimSmoothness);

			if (!m_bAnimate)
				timeoutHandlerConnection.disconnect ();

			pAnimateToggleButton->set_active (m_bAnimate);
			pAnimateToggleButton->signal_toggled().connect (sigc::ptr_fun (on_animate_toggled));
		}

		refXml->get_widget ("samplesTreeView", pSamplesTreeView);
		if (pSamplesTreeView)
		{
			m_pSampleListStore = new SampleListStore (pSamplesTreeView);

			// I was hoping to figure out how to set/highlight the first
			// row of the treeview... but even with asking on #gtk at
			// irc.gimp.org I'm not able to get this very simple thing done
			pSamplesTreeView->get_selection()->select (Gtk::TreePath ("0"));

			pSamplesTreeView->signal_row_activated().connect (sigc::ptr_fun (on_sample_activated));
		}

		refXml->get_widget ("mainDrawingArea", pMainDrawingArea);
		if (pMainDrawingArea)
		{
			m_pCairoSamples = new CairoSamples ();
			if (m_pCairoSamples)
			{
				m_pCairoSamples->set_cap_style (m_capStyle);
				m_pCairoSamples->set_join_style (m_joinStyle);
				m_pCairoSamples->set_operator (m_operator);
				m_pCairoSamples->set_extend_mode (m_extendMode);
				m_pCairoSamples->set_filter_mode (m_filterMode);
				m_pCairoSamples->set_line_width (m_fLineWidth);
				m_pCairoSamples->set_bg_color (m_bgColor);
				m_pCairoSamples->set_font_family (m_strFontFamily);
				m_pCairoSamples->set_font_size ((double) m_iFontSize * 0.01f);
				m_pCairoSamples->set_font_weight (m_fontWeight);
				m_pCairoSamples->set_font_slant (m_fontSlant);
				m_pCairoSamples->set_some_text (m_strSomeText);
				m_pCairoSamples->set_image_file_name (m_strImageFileName);
				m_pCairoSamples->set_svg_file_name (m_strSvgFileName);
				m_pCairoSamples->add_samples_to_list_store (m_pSampleListStore);
			}

			pMainDrawingArea->signal_expose_event().connect (sigc::ptr_fun (on_expose_event));
		}

		refXml->get_widget ("fontFontButton", pFontFontButton);
		if (pFontFontButton)
		{
			// assemble string describing the initial font
			std::string strFontName;
			strFontName = m_strFontFamily;
			switch (m_fontWeight)
			{
				case CAIRO_FONT_WEIGHT_NORMAL :
					strFontName.append (" Regular");
				break;

				case CAIRO_FONT_WEIGHT_BOLD :
					strFontName.append (" Bold");
				break;
			}
			switch (m_fontSlant)
			{
				case CAIRO_FONT_SLANT_ITALIC :
					strFontName.append (" Italic");
				break;

				case CAIRO_FONT_SLANT_OBLIQUE :
					strFontName.append (" Oblique");
				break;

				default :
				break;
			}
			std::stringstream int2str;
			int2str << m_iFontSize;
			strFontName.append (" ");
			strFontName.append (int2str.str ());

			pFontFontButton->set_font_name (strFontName);
			pFontFontButton->set_show_size (true);
			pFontFontButton->set_use_font (false);
			pFontFontButton->set_use_size (false);
			pFontFontButton->set_show_style (false);
			pFontFontButton->signal_font_set().connect (sigc::ptr_fun (on_font_set));
		}

		refXml->get_widget ("imageFileFileChooserButton", pImageFileFileChooserButton);
		if (pImageFileFileChooserButton)
		{
			pImageFileFileChooserButton->set_filename (m_strImageFileName.c_str ());
			Gtk::FileFilter pngFilter;
			pngFilter.set_name ("PNG Images");
			pngFilter.add_pattern ("*.png");
			pngFilter.add_pattern ("*.PNG");
			pImageFileFileChooserButton->add_filter (pngFilter);
			pImageFileFileChooserButton->set_filter (pngFilter);

			pImageFileFileChooserButton->signal_selection_changed ().connect (sigc::ptr_fun (on_image_selection_changed));
		}

		refXml->get_widget ("svgFileFileChooserButton", pSvgFileFileChooserButton);
		if (pSvgFileFileChooserButton)
		{
			pSvgFileFileChooserButton->set_filename (m_strSvgFileName.c_str ());
			Gtk::FileFilter svgFilter;
			svgFilter.set_name ("SVG Drawings");
			svgFilter.add_pattern ("*.svg");
			svgFilter.add_pattern ("*.SVG");
			pSvgFileFileChooserButton->add_filter (svgFilter);
			pSvgFileFileChooserButton->set_filter (svgFilter);

			pSvgFileFileChooserButton->signal_selection_changed ().connect (sigc::ptr_fun (on_svg_selection_changed));
		}

		refXml->get_widget ("animSmoothnessHScale", pAnimSmoothnessHScale);
		if (pAnimSmoothnessHScale)
		{
			pAnimSmoothnessHScale->set_value (m_iAnimSmoothness);
			pAnimSmoothnessHScale->signal_value_changed().connect (sigc::ptr_fun (on_anim_smoothness_changed));
		}

		refXml->get_widget ("textEntry", pTextEntry);
		if (pTextEntry)
		{
			pTextEntry->set_text (m_strSomeText);
			pTextEntry->signal_activate().connect (sigc::ptr_fun (on_text_entry_activated));
		}

		timer.start ();
		app.run (*pMainWindow);
		timer.stop ();
	}

	return 0;
}
