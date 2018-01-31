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

#include "sample-list-store.h"

SampleListStore::SampleListStore (Gtk::TreeView* pTreeView)
{
	m_pTreeView = pTreeView;
	m_refListStore = Gtk::ListStore::create (m_columns);
	m_pTreeView->set_model (m_refListStore);
	add_columns ();
}

SampleListStore::~SampleListStore ()
{
}

void SampleListStore::add_item (std::string strName)
{
	Gtk::TreeRow entry = *(m_refListStore->append ());
	entry[m_columns.name] = strName;
}

void SampleListStore::add_columns ()
{
	Gtk::CellRendererText* pRenderer = Gtk::manage (new Gtk::CellRendererText ());

	int cols_count = m_pTreeView->append_column ("Sample Name", *pRenderer);
	Gtk::TreeViewColumn* pColumn = m_pTreeView->get_column (cols_count - 1);

	pColumn->add_attribute (pRenderer->property_text (), m_columns.name);
}
