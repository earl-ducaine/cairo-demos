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

#ifndef _SAMPLELISTSTORE_H_
#define _SAMPLELISTSTORE_H_

#include <string>
#include <vector>
#include <gtkmm.h>

class SampleListStore
{
	public:
		SampleListStore (Gtk::TreeView* pTreeView);
		~SampleListStore ();
		void add_item (std::string strName);

	private:
		void add_columns ();

		struct ModelColumns : public Gtk::TreeModelColumnRecord
		{
			Gtk::TreeModelColumn<std::string> name;

			ModelColumns ()
			{
				add (name);
			}
		};

		Gtk::TreeView* m_pTreeView;
		const ModelColumns m_columns;
		Glib::RefPtr<Gtk::ListStore> m_refListStore;
};

#endif /*_SAMPLELISTSTORE_H_*/
