#!/usr/bin/env python
# -*- coding: utf-8 -*-
# generated by wxGlade 0.5 on Wed Oct 10 15:21:14 2007 from newHpiView

import wx

class Hpiview_Window(wx.Frame):
    def __init__(self, *args, **kwds):
        # begin wxGlade: Hpiview.__init__
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        wx.Frame.__init__(self, *args, **kwds)
        self.panel_2 = wx.Panel(self, -1)
        self.window_1 = wx.SplitterWindow(self, -1, style=wx.SP_3D|wx.SP_BORDER)
        self.window_1_pane_2 = wx.Panel(self.window_1, -1)
        self.window_1_pane_1 = wx.Panel(self.window_1, -1)
        self.notebook_1 = wx.Notebook(self, -1, style=0)
        self.notebook_1_pane_1 = wx.Panel(self.notebook_1, -1)
        self.panel_1 = wx.Panel(self, -1)
        
        # Menu Bar
        self.frame_2_menubar = wx.MenuBar()
        self.SetMenuBar(self.frame_2_menubar)
        wxglade_tmp_menu = wx.Menu()
        wxglade_tmp_menu.Append(101, "Quit", "", wx.ITEM_NORMAL)
        self.frame_2_menubar.Append(wxglade_tmp_menu, "Session")
        wxglade_tmp_menu = wx.Menu()
        wxglade_tmp_menu.Append(103, "Clear log", "", wx.ITEM_NORMAL)
        self.frame_2_menubar.Append(wxglade_tmp_menu, "Edit")
        wxglade_tmp_menu = wx.Menu()
        wxglade_tmp_menu.Append(105, "Discover", "", wx.ITEM_NORMAL)
        wxglade_tmp_menu.Append(106, "Load plugin", "", wx.ITEM_NORMAL)
        wxglade_tmp_menu.Append(107, "Unload plugin", "", wx.ITEM_NORMAL)
        self.frame_2_menubar.Append(wxglade_tmp_menu, "Action")
        wxglade_tmp_menu = wx.Menu()
        wxglade_tmp_menu.Append(109, "About", "", wx.ITEM_NORMAL)
        self.frame_2_menubar.Append(wxglade_tmp_menu, "Help")
        # Menu Bar end
        self.frame_2_statusbar = self.CreateStatusBar(1, 0)
        
        # Tool Bar
        self.frame_2_toolbar = wx.ToolBar(self, -1, style=wx.TB_HORIZONTAL|wx.TB_FLAT|wx.TB_TEXT|wx.TB_HORZ_LAYOUT|wx.TB_HORZ_TEXT)
        self.SetToolBar(self.frame_2_toolbar)
        self.frame_2_toolbar.AddLabelTool(201, "Close", wx.Bitmap("images/close.bmp", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "", "")
	#self.frame_2_toolbar.SetToolTipString("Close")
        self.frame_2_toolbar.AddLabelTool(202, "Discover", wx.Bitmap("images/discover.bmp", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "", "")
	#self.frame_2_toolbar.SetToolTipString("Discover")
        self.frame_2_toolbar.AddCheckLabelTool(203, "Subscribe events", wx.Bitmap("images/unsubscribe events.bmp", wx.BITMAP_TYPE_ANY), wx.NullBitmap, "", "")
        self.frame_2_toolbar.AddLabelTool(204, "Get events", wx.Bitmap("images/Get event.bmp", wx.BITMAP_TYPE_ANY), wx.NullBitmap, wx.ITEM_NORMAL, "", "")
        # Tool Bar end
        self.bitmap_button_1 = wx.BitmapButton(self.panel_1, -1, wx.Bitmap("images/Side toolbar1.bmp", wx.BITMAP_TYPE_ANY))
	self.bitmap_button_1.SetToolTipString("Hide domain list")
        self.bitmap_button_2 = wx.BitmapButton(self.panel_1, -1, wx.Bitmap("images/Side toolbar2.bmp", wx.BITMAP_TYPE_ANY))
	self.bitmap_button_2.SetToolTipString("Open new session")
        self.list_box_1 = wx.ListBox(self, -1, choices=[])
        self.tree_ctrl_1 = wx.TreeCtrl(self.notebook_1_pane_1, -1, style=wx.TR_HAS_BUTTONS|wx.TR_NO_LINES|wx.TR_DEFAULT_STYLE|wx.SUNKEN_BORDER)
        self.text_ctrl_1 = wx.TextCtrl(self.notebook_1_pane_1, -1, "", style=wx.TE_MULTILINE|wx.TE_RICH2|wx.TE_WORDWRAP|wx.TE_READONLY)
        self.list_ctrl_1 = wx.ListCtrl(self.window_1_pane_1, -1, style=wx.LC_REPORT|wx.SUNKEN_BORDER)
        self.text_ctrl_2 = wx.TextCtrl(self.window_1_pane_2, -1, "", style=wx.TE_MULTILINE|wx.TE_RICH2|wx.TE_WORDWRAP|wx.TE_READONLY)
        self.button_1 = wx.ToggleButton(self.panel_2, -1, "Messages")
	self.button_1.SetToolTipString("Messages")
        self.button_2 = wx.ToggleButton(self.panel_2, -1, "Events")
	self.button_2.SetToolTipString("Events")


        self.list_ctrl_1.InsertColumn(1,"Time",wx.LIST_FORMAT_LEFT,-1)
        self.list_ctrl_1.InsertColumn(1,"Source",wx.LIST_FORMAT_LEFT,-1)
        self.list_ctrl_1.InsertColumn(1,"Severity",wx.LIST_FORMAT_LEFT,-1)
        self.list_ctrl_1.InsertColumn(1,"EventType",wx.LIST_FORMAT_LEFT,-1)
        self.list_ctrl_1.InsertColumn(1,"Domain",wx.LIST_FORMAT_LEFT,-1)

        self.__set_properties()
        self.__do_layout()

        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: Hpiview.__set_properties
        self.SetTitle("OpenHpi View")
        self.frame_2_statusbar.SetStatusWidths([-1])
        # statusbar fields
        frame_2_statusbar_fields = ["Ready"]
        for i in range(len(frame_2_statusbar_fields)):
            self.frame_2_statusbar.SetStatusText(frame_2_statusbar_fields[i], i)
        self.frame_2_toolbar.SetToolBitmapSize((12, 12))
        self.frame_2_toolbar.Realize()
        self.bitmap_button_1.SetSize(self.bitmap_button_1.GetBestSize())
        self.bitmap_button_2.SetSize(self.bitmap_button_2.GetBestSize())
        self.panel_1.SetMinSize((60, 187))
        self.list_box_1.SetMinSize((150, 182))
        self.tree_ctrl_1.SetMinSize((200, 147))
        self.text_ctrl_1.SetMinSize((650, 152))
        self.notebook_1.SetMinSize((850, 182))
	self.SetMinSize((800,600))
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: Hpiview.__do_layout
        grid_sizer_1 = wx.FlexGridSizer(2, 1, 0, 0)
        grid_sizer_2 = wx.FlexGridSizer(2, 1, 0, 0)
        grid_sizer_5 = wx.FlexGridSizer(1, 3, 0, 0)
        grid_sizer_4 = wx.FlexGridSizer(1, 1, 0, 0)
        grid_sizer_3 = wx.FlexGridSizer(1, 1, 0, 0)
        grid_sizer_7 = wx.FlexGridSizer(1, 2, 0, 0)
        grid_sizer_9 = wx.FlexGridSizer(1, 2, 0, 0)
        grid_sizer_10 = wx.FlexGridSizer(1, 2, 0, 0)
        grid_sizer_11 = wx.FlexGridSizer(2, 1, 0, 0)
        grid_sizer_11.Add(self.bitmap_button_1, 0, 0, 0)
        grid_sizer_11.Add(self.bitmap_button_2, 0, 0, 0)
        self.panel_1.SetSizer(grid_sizer_11)
        grid_sizer_11.AddGrowableCol(0)
        grid_sizer_7.Add(self.panel_1, 1, wx.EXPAND, 0)
        grid_sizer_9.Add(self.list_box_1, 0, wx.EXPAND, 0)
        grid_sizer_10.Add(self.tree_ctrl_1, 1, wx.EXPAND, 0)
        grid_sizer_10.Add(self.text_ctrl_1, 0, wx.EXPAND, 0)
        self.notebook_1_pane_1.SetSizer(grid_sizer_10)
        grid_sizer_10.AddGrowableRow(0)
        grid_sizer_10.AddGrowableCol(0)
        self.notebook_1.AddPage(self.notebook_1_pane_1, "DEFAULT")
        grid_sizer_9.Add(self.notebook_1, 1, wx.EXPAND, 0)
        grid_sizer_9.AddGrowableRow(0)
        grid_sizer_9.AddGrowableCol(0)
        grid_sizer_7.Add(grid_sizer_9, 1, wx.EXPAND, 0)
        grid_sizer_7.AddGrowableRow(0)
        grid_sizer_7.AddGrowableCol(0)
        grid_sizer_1.Add(grid_sizer_7, 1, wx.EXPAND, 0)
        grid_sizer_3.Add(self.list_ctrl_1, 1, wx.EXPAND, 0)
        self.window_1_pane_1.SetSizer(grid_sizer_3)
        grid_sizer_3.AddGrowableRow(0)
        grid_sizer_3.AddGrowableCol(0)
        grid_sizer_4.Add(self.text_ctrl_2, 0, wx.EXPAND, 0)
        self.window_1_pane_2.SetSizer(grid_sizer_4)
        grid_sizer_4.AddGrowableRow(0)
        grid_sizer_4.AddGrowableCol(0)
        self.window_1.SplitVertically(self.window_1_pane_1, self.window_1_pane_2)
        grid_sizer_2.Add(self.window_1, 1, wx.EXPAND, 0)
        grid_sizer_5.Add(self.button_1, 0, 0, 0)
        grid_sizer_5.Add(self.button_2, 0, 0, 0)
        grid_sizer_5.Add((300, 20), 0, 0, 0)
        self.panel_2.SetSizer(grid_sizer_5)
        grid_sizer_2.Add(self.panel_2, 1, wx.EXPAND, 0)
        grid_sizer_2.AddGrowableRow(0)
        grid_sizer_2.AddGrowableCol(0)
        grid_sizer_1.Add(grid_sizer_2, 1, wx.EXPAND, 0)
        self.SetSizer(grid_sizer_1)
        grid_sizer_1.Fit(self)
        grid_sizer_1.AddGrowableRow(0)
        grid_sizer_1.AddGrowableCol(0)


	self.window_1.Show(False)
	self.notebook_1.Show(False)

        self.Layout()
        # end wxGlade
