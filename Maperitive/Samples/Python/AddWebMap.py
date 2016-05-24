# -*- coding: utf-8 -*-

# Sample showing how to add a standard web map

from maperipy import Map
from maperipy.webmaps import WebMap

Map.clear()
Map.add_web_map(WebMap.MapQuestOsm)
