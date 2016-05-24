# -*- coding: utf-8 -*-

# Sample script which demonstrates filtering of OSM data from one OSM file
# and then storing the filtered data into another OSM file.

# Author: Igor Brejc
# License: public domain

from maperipy import *
from maperipy.osm import *

# Load the OSM data from a file.
original = OsmData.load_xml_file("D:/mydata/some_big_file.osm")

# Create another (empty) OSM dataset where we will send the interesting things.
filtered = OsmData()

# Find all coastline ways...
for way in original.find_ways(lambda x : x.has_tag("natural", "coastline")):
    # ...and copy OSM nodes for these ways into the filtered dataset.
    for node_id in way.nodes:
        # (Note that we must first check that the node is actually in the storage.
        # Some OSM extracts do not contain all referenced nodes!)
        if original.has_node(node_id):
            filtered.add_node(original.node(node_id))

    # Also copy the OSM way itself.
    filtered.add_way(way)

# Now save the filtered dataset into another XML file.
filtered.save_xml_file("D:/mydata/costlines_only.osm")