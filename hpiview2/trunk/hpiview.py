#!/usr/bin/env python
import wxversion
wxversion.ensureMinimal("2.6")
import wx
from openhpi_view.hpiview_window import *
from openhpi_view.hpiview_callbacks import *


if __name__ == "__main__":
    app = wx.PySimpleApp(0)
    wx.InitAllImageHandlers()
    frame_1 = Hpiview_Window(None, -1, "")
    callback = Hpiview_Callbacks(frame_1)
    app.SetTopWindow(frame_1)
    frame_1.Show()
    app.MainLoop()


