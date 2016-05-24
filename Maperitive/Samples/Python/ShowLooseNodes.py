# -*- coding: utf-8 -*-

# Sample script which demonstrates some ways of querying the OSM data.
# The script looks for the first OSM map source and then finds all
# OSM nodes which do not have any tags and do not belong to any ways or relations.

# Author: Igor Brejc
# License: public domain

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

# We store all the loose node IDs into a set.
loose_nodes = set()

# First find all nodes that do not have any tags...
for node in osm.find_nodes (lambda x : len(x.tags) == 0):
    loose_nodes.add(node.id)

# ... Now remove any of such nodes that belong to OSM ways...
for way in osm.find_ways(lambda x : True):
    for node_id in way.nodes:
        if node_id in loose_nodes:
            loose_nodes.remove(node_id)

# ... And also remove any nodes that belong to relations.
for relation in osm.find_relations(lambda x : True):
    for member in relation.members:
        if member.ref_type == OsmReferenceType.NODE and member.ref_id in loose_nodes:
            loose_nodes.remove(member.ref_id)

# Now we prepare a list of points to be displayed on the map.
# Each point represents one loose node.
points = []
for node_id in loose_nodes:
    node = osm.node(node_id)
    points.append(node.location)

# Now we create a custom map layer...
layer = Map.add_custom_layer()
# And we create a point symbol that will draw circles 
# for all the mentioned points.
symbol = PointShapeSymbol("loose nodes", Srid.Wgs84LonLat, points)
symbol.shape_size = 6
symbol.style.fill_color = Color("red")

# Finally we add the symbol to the newly created layer.
layer.add_symbol(symbol)
