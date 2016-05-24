# -*- coding: utf-8 -*-

# Draws a "circle" of a specified distance from a specified location.

# Author: Igor Brejc
# License: public domain

from maperipy import *
from maperipy.osm import *
import math

# Set your location here...
location = Point(15.6530555556, 46.5575)
# And the distance (in meters!)
distance = 3000

# Create a custom map layer...
layer = Map.add_custom_layer()

# Symbol for the circle
symbol = PolygonSymbol("circle", Srid.Wgs84LonLat)
symbol.style.pen_width = 8
symbol.style.pen_color = Color("blue")
symbol.style.pen_opacity = 0.5
symbol.style.fill_opacity = 0

# Calculate circle's points
earth_circumference = 6371000 * math.pi * 2;
points = []
arc = distance / earth_circumference * 360;

for angle in range(0, 360):
    radians = math.radians(angle)
    y = location.y + math.sin(radians) * arc
    lat_factor = math.cos(math.radians(y))
    x = location.x + math.cos(radians) * arc / lat_factor
    points.append(Point(x,y))

# Construct a polygon representing the circle
circle = Polygon(LinearRing(points))
symbol.add(circle)

layer.add_symbol(symbol)
