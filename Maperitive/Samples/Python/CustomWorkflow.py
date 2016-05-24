# -*- coding: utf-8 -*-

# Shows some simple capabilities of changing the default map painting workflow.

# Author: Igor Brejc
# License: public domain

# The sample scripts tells Maperitive to skip rendering map text labels.
# The cycles_to_output() method specifies the starting and ending paint cycles (the ending
# cycle will not be included).

# Here's a list of the default painting cycle values:
# 	MapBackground = 100
# 	Landmass = 200
# 	BitmapBackground = 300
# 	LandUse = 400
# 	BitmapForeground = 500
# 	Infrastructure = 600
# 	Symbols = 700
# 	Labels (linear) = 799
# 	Labels (horizontal) = 800
# 	BitmapOverlay = 900
# 	Highlights = 950
# 	MapLegend = 1000

from maperipy import Map

# This effectively skips cycles 799 and 800
with Map.init_workflow() as workflow:
	workflow.cycles_to_output(0, 799).cycles_to_output(801, 1001)
