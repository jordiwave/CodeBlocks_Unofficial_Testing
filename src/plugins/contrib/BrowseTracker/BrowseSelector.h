/*
	This file is part of Browse Tracker, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef BROWSE_SELECTOR_H
#define BROWSE_SELECTOR_H

#include "scrollingdialog.h"
#include <map>
#include <wx/bitmap.h>

class wxListBox;
class wxAuiNotebook;
class wxPanel;
class BrowseTracker;

// ----------------------------------------------------------------------------
class BrowseSelector : public wxScrollingDialog
// ----------------------------------------------------------------------------
{
    protected:
        wxListBox * m_listBox;
        long        m_selectedItem;
        std::map<int, int> m_indexMap;
        wxPanel  *  m_panel;
        static wxBitmap m_bmp;

        void CloseDialog();

    public:
        /**
         * Parameterized constructor
         * \param parent dialog parent window
         */
        BrowseSelector(wxWindow * parent, BrowseTracker * pBrowseTracker, int menuID);

        /**
         * Destructor
         */
        virtual ~BrowseSelector();

        /**
         * Create the dialog, usually part of the two steps construction of a
         * dialog
         * \param parent dialog parent window
         */
        void Create(wxWindow * parent, BrowseTracker * pBrowseTracker, int menuID);

        /// Event handling
        void OnKeyUp(wxKeyEvent & event);
        void OnKeyDown(wxKeyEvent & event);
        void OnNavigationKey(wxKeyEvent & event);
        void OnItemSelected(wxCommandEvent & event);
        void OnPanelPaint(wxPaintEvent & event);
        void OnPanelEraseBg(wxEraseEvent & event);
        void OnWindowKillFocus(wxFocusEvent & event);
        int  PopulateListControl(EditorBase * pEditor);

    private:
        /**
         * Default constructor
         */
        BrowseSelector();
        /**
         * Brighten a given colour with amount
         * \param color starting colour
         * \param percent percent, 0 - no change, 100 - white
         * \return brighten colour
         */
        wxColor LightColour(const wxColour & color, int percent);

        /**
         * Paint rectangle with gradient colouring
         * \param dc device context
         * \param rect rectangle
         * \param startColor gradient colour 1
         * \param endColor gradient colour 2
         * \param vertical use vertical gradient or horizontal
         */
        void PaintStraightGradientBox(wxDC & dc, const wxRect & rect, const wxColour & startColor, const wxColour & endColor, bool  vertical = true);

        BrowseTracker * m_pBrowseTracker;
        int             m_menuID;
        int             m_KeyDownCode;
        int             m_KeyDownMods;

};

#endif // #ifndef BROWSE_SELECTOR_H

