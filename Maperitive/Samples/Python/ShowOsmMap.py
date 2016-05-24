# -*- coding: utf-8 -*-

# Sample script which shows how to show an OSM vector source 
# and how to set your own rules file.

# Author: Igor Brejc
# License: public domain

from maperipy import *
from maperipy.rendering_rules import Rules
from maperipy.osm import *

Map.clear()

# Load the rules...
rules = Rules.load_from_file(r"Rules\urbanight.mrules")

# ... and set them as default rules for the map.
Map.rules = rules

# And now load your own OSM map source (you need to change the path!)
osm_layer = Map.add_osm_source(r"alice\wonderland.osm")
