# -*- coding: utf-8 -*-

# The utility script which checks the OSM coastline data and displays
# color-coded results:
# - green coastlines: these are valid (closed) polygons and are OK.
# - red coastlines: invalid polygons (missing points which prevent the polygon from being closed).
# - pink coastlines: these coastlines have missing ends, Maperitive extended them artificially.
# - black coastlines: these occur if the whole processing has been aborted (due to some data or algorithm error).

# Author: Igor Brejc
# License: public domain

# How to use:
# 1. load your OSM file into Maperitive.
# 2. run this script and view results.

from maperipy import *
from maperipy.osm import *

osm = None

# Look for the first OSM map source.
for layer in Map.layers:
    if layer.layer_type == "OsmLayer":
        osm = layer.osm
        break

# If there are no OSM map sources, report an error...
if osm == None:
    raise AssertionError("There are no OSM map souces.")

processor = CoastlineProcessor()
result = processor.run(osm)

if not result.success:
    print "Coastline processing failed, error code is {0}".format(result.error_code)

# Now we create a custom map layer...
layer = Map.add_custom_layer()
# Hack to print the legend.
layer.attribution = "green: valid (closed) | red: invalid (unclosed) | pink: had to be extended | black: unprocessed | orange: OSM download bounds";

# Symbol for showing coastline processing bounds.
bounds_symbol = PolygonSymbol("coastline bounds", Srid.Wgs84LonLat)
bounds_symbol.style.pen_width = 2
bounds_symbol.style.pen_color = Color("blue")
bounds_symbol.style.line_style = LineStyle.DOT
bounds_symbol.style.fill_opacity = 0

# Add the bboxes.
for bbox in result.bboxes:
    bounds_symbol.add(bbox.polygon)

# Symbol for showing OSM data bounds.
osm_bounds_symbol = PolygonSymbol("OSM data bounds", Srid.Wgs84LonLat)
osm_bounds_symbol.style.pen_width = 7
osm_bounds_symbol.style.pen_color = Color("orange")
osm_bounds_symbol.style.pen_opacity = 0.3
osm_bounds_symbol.style.line_style = LineStyle.SOLID
osm_bounds_symbol.style.fill_opacity = 0

# Add the bboxes.
for bbox in osm.bboxes:
    osm_bounds_symbol.add(bbox.polygon)

# Now add various coastline types.
unprocessed_symbol = LineSymbol("unprocessed coastlines", Srid.Wgs84LonLat)
unprocessed_symbol.style.pen_width = 2
unprocessed_symbol.style.pen_color = Color("black")
unprocessed_symbol.paint_cycle = 1000;

invalid_symbol = LineSymbol("invalid coastlines", Srid.Wgs84LonLat)
invalid_symbol.style.pen_width = 2
invalid_symbol.style.pen_color = Color("red")
invalid_symbol.paint_cycle = 1000;

extended_symbol = LineSymbol("extended coastlines", Srid.Wgs84LonLat)
extended_symbol.style.pen_width = 2
extended_symbol.style.pen_color = Color("pink")
extended_symbol.paint_cycle = 1000;

valid_symbol = LineSymbol("valid coastlines", Srid.Wgs84LonLat)
valid_symbol.style.pen_width = 2
valid_symbol.style.pen_color = Color("green")
valid_symbol.paint_cycle = 1000;

for coastline in result.coastlines:
    if not coastline.was_processed:
        unprocessed_symbol.add(coastline.geom)
    elif coastline.was_extended:
        extended_symbol.add(coastline.geom)
    elif not coastline.is_closed:
        invalid_symbol.add(coastline.geom)
    else:
        valid_symbol.add(coastline.geom)

# Finally we add symbols to the newly created layer.
layer.add_symbol(osm_bounds_symbol)
layer.add_symbol(bounds_symbol)
layer.add_symbol(unprocessed_symbol)
layer.add_symbol(valid_symbol)
layer.add_symbol(extended_symbol)
layer.add_symbol(invalid_symbol)
