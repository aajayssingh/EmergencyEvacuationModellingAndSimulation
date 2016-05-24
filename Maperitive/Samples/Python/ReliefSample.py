# -*- coding: utf-8 -*-

# Sample script which demonstrates generating hillshading and relief contours.

# Author: Igor Brejc
# License: public domain

from maperipy import *
from maperipy.contours import ContourSymbol
from maperipy.relief import HillShadingProcessor
from maperipy.relief import IgorHillshader
from maperipy.relief import ReliefUtils

if Map.coord_srid != Srid.Wgs84LonLat:
    raise AssertionError("Your map needs to use WGS84 SRS in order to be able to run this script.")    

# A function for styling major contour lines. It is called once per map rendering.
def contours_style_major(style, context):
    # Here we demonstrate how to set a zoom-dependent pen width
    style.pen_width = context.zoom_calc("12:1;13:2")
    return True

# A function for styling major contour labels. It is called once per map rendering.
def label_style_major(style, context):
    style.font_italic = True
    style.font_size = 10
    style.text_outline_width = 2
    return True

# A function for styling minor contour lines. It is called once per map rendering.
def contours_style_minor(style, context):
    # Here we demonstrate how to prevent rendering of minor contours on low zooms
    if context.zoom < 13:
        return False
    style.pen_width = 1
    return True

# We define the area based on the current map view.
relief_bbox = Map.visible_bounds

if relief_bbox.delta_x > 5 or relief_bbox.delta_y > 5:
    raise AssertionError("Your current map view is too big ({0}x{1} degress), please reduce it too 5x5.".format(relief_bbox.delta_x, relief_bbox.delta_y))

# We create a custom map layer for contours and hillshading.
custom_layer = Map.add_custom_layer()

# The color we will use for the contour lines and labels
contour_color = Color("#511900")

# Fetch SRTM3 DEM for that area...
dem = ReliefUtils.nasa_srtmv3r3.fetch_dem(relief_bbox)

# ... and now generate contours from that DEM (with 20m interval)
contours = ReliefUtils.generate_contours(dem, relief_bbox, 20)

# Now we create a symbol that will show that contours
symbol = ContourSymbol("my contours", contours)

# Some mumbo-jumbo for labeling algorithm.
symbol.set_annealing_schedule(0.9, 300, 300, 5)

# Here we define the first contour style (for major contours)
# We use 100m interval for major ones.
style = symbol.add_group(100, contours_style_major, label_style_major);
style.line_style.pen_color = contour_color
style.label_style.font_color = contour_color
style.label_coverage_range = 200

# Here we define the second contour style (for minor contours)
# We use 20m interval for minor ones.
style = symbol.add_group(20, contours_style_minor)
style.line_style.pen_color = contour_color

# Add our contours symbol to the custom layer.
custom_layer.add_symbol(symbol)

# Now we create a hillshading bitmap for the same area.
# We use intensity of 2 and the blue color.
bitmap = HillShadingProcessor.shade(dem, relief_bbox, Map.proj_srid, 1, IgorHillshader(), Color("blue"), 2)

# Here we create a RasterSymbol for our bitmap...
bitmap_symbol = RasterSymbol("my hillshading", bitmap.srid)
# ... and add the bitmap to the symbol.
bitmap_symbol.add(bitmap)

# Finally we add our raster symbol to the custom layer.
custom_layer.add_symbol(bitmap_symbol)
