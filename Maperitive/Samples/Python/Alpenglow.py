# -*- coding: utf-8 -*-

# Generates a hillshading with alpenglow effect.
# See http://www.shadedrelief.com/illumination/index.html for more info.
# There are several parameters you can fool around with to change the effect, 
# see the comments below.
# This script also demonstrates how you can write your own shaders.

# Author: Igor Brejc
# License: public domain

from maperipy import *
from maperipy.relief import *
import math

# The direction of the sunlight - 0 is from the east, 135 is fron NW.
sun_azimuth = math.radians(135)

# The vertical angle of the sun, measured from the horizon.
sun_elevation = math.radians(35)

# The minimum elevation (in meters) at which the alpenglow starts to appear.
min_alpenglow_elevation = 1000
# The elevaton (in meters) at which the alpenglow reaches its full effect.
max_alpenglow_elevation = 2800

# The base color of the hillshade.
shade_base_color = Color ("#426463")
# The color of the fully illuminated slope.
light_color = Color("white")
# The color of the alpenglow effect.
alpenglow_color = Color (255, 250, 255, 200);

# Calculate some constants before we do the work.
minDirection = -0.64
delta_alpenglow_elevation = max_alpenglow_elevation - min_alpenglow_elevation

lx = math.cos (sun_azimuth) * math.cos (sun_elevation)
ly = math.sin (sun_azimuth) * math.cos (sun_elevation)
lz = math.sin (sun_elevation)

# Helper function for calculating the dot product of two 3D vectors.
def dotproduct(x1, y1, z1, x2, y2, z2):
    return x1 * x2 + y1 * y2 + z1 * z2

# The actual shader function which is called for each pixel of the hillshading bitmap.
# It receives the elevation, the aspect (orientation) of the slope and the steepnes (slope).
def alpenglow(elevation, aspect, slope):

    sin_slope = math.sin (slope)

    nx = math.cos(aspect) * sin_slope
    ny = math.sin(aspect) * sin_slope
    nz = math.cos(slope)

    direction = dotproduct(nx, ny, nz, lx, ly, lz)

    direction = (direction - minDirection) / (1 - minDirection);
    if direction < 0:
        direction = 0

    # This is how we can mix two colors together.
    shade_color = shade_base_color.mix (light_color, direction)

    if direction > 0.0 and elevation > min_alpenglow_elevation:
        elevation_factor = min((elevation - min_alpenglow_elevation) / delta_alpenglow_elevation, 1);

        alpenglow_factor = math.pow(direction, 4)
        
        return shade_color.mix (alpenglow_color, elevation_factor * alpenglow_factor).argb;

    return shade_color.argb;


if Map.coord_srid != Srid.Wgs84LonLat:
    raise AssertionError("Your map needs to use WGS84 SRS in order to be able to run this script.")    

# We define the area somewhere interesting in the Alps.
relief_bbox = BoundingBox(Srid.Wgs84LonLat, 7.49059548411773,46.3534927253604,8.03995765925068,46.6137318802106)

# We create a custom map layer for contours and hillshading.
custom_layer = Map.add_custom_layer()

# Fetch Viewfinder's Alps DEM for that area...
print "Fetching the DEM..."

dem = ReliefUtils.nasa_srtmv3r3.fetch_dem(relief_bbox)

# Execute the hillshading.
print "Generating the hillshading..."
bitmap = ReliefUtils.generate_relief_bitmap(dem, relief_bbox, Srid.WebMercator, 1, alpenglow)

# Here we create a RasterSymbol for our bitmap...
bitmap_symbol = RasterSymbol("my hillshading", bitmap.srid)
# ... and add the bitmap to the symbol.
bitmap_symbol.add(bitmap)

# Finally we add our raster symbol to the custom layer.
custom_layer.add_symbol(bitmap_symbol)

# Zoom to the hillshading.
Map.zoom_area(relief_bbox)
