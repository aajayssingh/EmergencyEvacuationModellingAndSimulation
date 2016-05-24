# -*- coding: utf-8 -*-

# Sample script which demonstrates how to work with custom DEM in maperipy.
# The script downloads a DEM tile of a small part of Alps from http://www.viewfinderpanoramas.org/.
# It then generates a hillshading using this DEM tile.

# Author: Igor Brejc
# License: public domain

from maperipy import *
from maperipy.relief import HillShadingProcessor
from maperipy.relief import IgorHillshader
from maperipy.relief import ReliefUtils
from maperipy.relief import CustomSrtm1Source
import os
import urllib
import zipfile

if Map.coord_srid != Srid.Wgs84LonLat:
    raise AssertionError("Your map needs to use WGS84 SRS in order to be able to run this script.")    

# The subdirectory where we will store the DEM data.
dem_dir = 'viewfinder'

if not os.path.exists(dem_dir):
    os.makedirs(dem_dir)

# Download the tile if it's not already on the disk.
if not os.path.exists('viewfinder/N46E010.hgt'):
    zip_name = 'viewfinder/N46E010.zip'

    # Download a sample DEM tile from Viewfinder
    print 'Downloading the DEM zip file...'
    urllib.urlretrieve("http://www.viewfinderpanoramas.org/dem1/N46E010.zip", zip_name)

    print 'Extracting the DEM tile from the zip file...'
    myzip = zipfile.ZipFile(zip_name, 'r')
    myzip.extractall(dem_dir)
    myzip.close()

# Create a custom DEM source which uses the specific disk directory.
dem_source = CustomSrtm1Source(dem_dir)

# We define the area of interest.
relief_bbox = BoundingBox(Srid.Wgs84LonLat, 10.1, 46.2, 10.8, 46.8)

# Here we create a custom map layer for contours and hillshading.
custom_layer = Map.add_custom_layer()

# Fetch DEM for that area...
dem = dem_source.fetch_dem(relief_bbox)

# Make sure the DEM raster is not empty
if dem.is_empty:
    raise AssertionError("The DEM is empty.")    

# Now we create a hillshading bitmap for the same area.
# We use intensity of 2 and the blue color.
bitmap = HillShadingProcessor.shade(dem, relief_bbox, Map.proj_srid, 1, IgorHillshader(), Color("blue"), 2)

# Here we create a RasterSymbol for our bitmap...
bitmap_symbol = RasterSymbol(bitmap.srid)
# ... and add the bitmap to the symbol.
bitmap_symbol.add(bitmap)

# Finally we add our raster symbol to the custom layer.
custom_layer.add_symbol(bitmap_symbol)

# Zoom to the hillshading.
Map.zoom_area(custom_layer.bounding_box)
