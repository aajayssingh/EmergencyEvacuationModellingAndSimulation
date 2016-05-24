# -*- coding: utf-8 -*-

# Sample showing how to add an offline web map.
# NOTE: in order for this to work, you need to place the web tiles into the Cache/WebTiles/MyOfflineMap
# directory ('MyOfflineMap' is the provider name, you can change that, see below). The file structure 
# is the same as with the standard Web maps.

# The easiest way to test this is to copy the Cache/WebTiles/OSM Mapnik directory to MyOfflineMap
# and run this script. You will be show the standard OSM web map tiles, but only those that have 
# been cached on your disk, with no extra tiles downloading from the web.

# Author: Igor Brejc
# License: public domain

from maperipy import Map
from maperipy.webmaps import WebMapLayer

Map.clear()

# We create an offline web map layer, with the name of the layer specified. This name is then used
# to find the tiles in the cache (Cache/WebTiles directory).
offline_map = WebMapLayer.create_offline("MyOfflineMap")
# Now we add that layer to the map.
Map.add_layer(offline_map)
