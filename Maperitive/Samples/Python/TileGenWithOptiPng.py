# -*- coding: utf-8 -*-

# A sample script which generates tiles and then optimizes them using OptiPNG
# tool (which is included in Maperitive package).

# Author: Igor Brejc
# License: public domain

# Instructions:
# 1. Run the script (drag & drop it into Maperitive or using the run-python command).

from maperipy import App
from maperipy import Map
from maperipy.webmaps import Tile
from maperipy.tilegen import TileGenCommand

def func1 (zoom, x, y, tiles_horizontal, tiles_vertical):
    # Demonstrates how to control which tiles get rendered
    # note that Maperitive generates tiles in batches
    # (defined by tiles_horizontal x tiles_vertical count).
    # So for example, 3x3 (=9) tiles are generated in a single bitmap 
    # and then later they are split into individual tiles.

    # In this case we just print some info to Maperitive's command log window.
    print "Generating tile batch: {0}/{1}/{2} ({3}x{4} tiles)" \
        .format(zoom, x, y, tiles_horizontal, tiles_vertical)

    # In this case we return True to signal that we want 
    # these tiles to be rendered.
    return True

def func2 (tile):
    # Demonstrates how to control the saving of a tile to disk.
    # At this point we could do some postprocessing of the tile bitmap
    # or store it somewhere else (and then return False to prevent
    # Maperitive from saving it to the default location).
    print "Saving tile: {0}/{1}/{2} (file: {3})" \
        .format(tile.zoom, tile.tile_x, tile.tile_y, tile.file_name)

    # You could, for example, store tiles into a database instead
    # of writing them to disk.

    # In this case we return True to signal that we want 
    # the tile to be saved to the default location.
    return True    

def optimize_tiles():
    args = []

    # Turn on backup files (just to compare sizes with optimized tiles).
    #args.append ("-backup")

    # Set the OptiPNG optimization rate (0 to 7), 7 being very slow.
    optimization_rate = 2
    args.append ("-o{0}".format(optimization_rate))

    # We don't want any OptiPNG output text to clutter Maperitive.
    args.append ("-quiet")

    args.extend(tiles_to_optimize)

    # This is one way to execute external programs.
    App.run_program("tools/OptiPNG/optipng.exe", 100, args)

    # Clear the list.
    del tiles_to_optimize[:]

def collect_tiles (file_name):
    # Demonstrates how to collect the list of generated tiles.
    # You could, for example, use this function to upload
    # tiles to a Web server.

    # In this case we collect the file names to a list...
    tiles_to_optimize.append("\"{0}\"".format(file_name))

    # ... and then we call OptiPNG when the list gets full.
    if len(tiles_to_optimize) >= 10:
        optimize_tiles()

if App.is_mono:
    raise AssertionError("Sorry, this script does not work when running on Linux or Mac.")

tiles_to_optimize = []

# Generate tiles for the currently set geometry bounds and the current zoom level
min_zoom = Map.position.zoom
max_zoom = Map.position.zoom
cmd = TileGenCommand(Map.geo_bounds, min_zoom, max_zoom)

# This is how we override TileGenCommand with custom functions.
cmd.tile_generation_filter = func1
cmd.tile_save_filter = func2
cmd.after_tile_save = collect_tiles

cmd.execute()

# Run the optimization again for any left-over tiles.
optimize_tiles()
