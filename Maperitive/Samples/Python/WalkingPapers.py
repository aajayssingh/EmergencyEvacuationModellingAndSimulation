# -*- coding: utf-8 -*-

# A script which splits the user-defined printing bounds into a grid of pages
# and then generates bitmaps for each of the page (with the page ID marked
# on the top-left corner of the bitmap).

# The functionality is similar to http://walking-papers.org/, but with few added extras:
# 1. You can run it on your own vector data and styles.
# 2. The printing bounds of the map can be of any size and shape - not just A4/A3.
# 3. You can specify page overlaps, see below.

# Author: Igor Brejc
# License: public domain

# Instructions:
# 1. In Maperitive GUI place the printing bounds on the area you're interested in.
# 2. Edit the script: set your map scale, paper size, orientation and margins (see below).
# 3. Run the script (drag & drop it into Maperitive or using the run-python command).
# NOTE: if your walking papers produce more than 30 pages, edit the page_breaks_filter 
# function.

from maperipy import Alignment
from maperipy import App
from maperipy import Color
from maperipy import Map
from maperipy import Margins
from maperipy import PaintStyle
from maperipy import Paper
from maperipy import PaperOrientation
from maperipy.commands import ExportBitmapCommand

def page_breaks_filter (cols, rows):
    # This is just to prevent too many pages being produced by mistake.
    # If you want to generate a larger number of pages, replace 30 with 
    # a higher value.
    print "Walking papers export requires {0}x{1} pages.".format(cols, rows)
    return cols * rows <= 30

def export_page (col, row, page_print_bounds):
    """Export the specified page to a bitmap file."""
    # The function which is called for each individual page.
    # In our case we are adding a page ID text on the top left corner
    # and then exporting the page to a bitmap.
    col_char = chr(ord('A') + col)
    page_id = "{0}{1}".format(col_char, row+1)

    add_page_id_decoration(page_id)

    cmd = ExportBitmapCommand()
    cmd.print_bounds = page_print_bounds
    cmd.bitmap_file = "output/walking_papers_{0}.png".format(page_id)
    cmd.execute()

def add_page_id_decoration(page_id):
    """Add the page ID decoration to the map."""
    Map.add_decoration("page_id", lambda c : paint_page_id(page_id, c))

def paint_page_id(page_id, context):
    """Print the page ID on the top left corner of the exported page bitmap."""
    style = PaintStyle()
    style.text_outline_width = 0
    style.font_family = "Arial"
    style.font_bold = True
    style.font_size = 96
    style.font_color = Color("#9000FF")
    style.font_opacity = 0.5
    context.set_style(style)

    context.draw_text(page_id, 20, 20, 10000, Alignment.NEAR, Alignment.NEAR)

def remove_page_id_decoration():
    """Remove the grid page ID decoration from the map."""
    Map.remove_decoration("page_id")

bounds = Map.print_bounds

if bounds == None:
    raise AssertionError("Printing bounds are not set.")

# The map scale to use for individual pages. SET THIS BEFORE RUNNING THE SCRIPT!
# Note that the actual map scale may vary 
# for each individual page. This is due to the Mercator projection 
# depending on the latitude.
map_scale = 3000000

# Set the paper size for individual pages.
paper = Paper("A4")
# Only A4 is currently supported, but you can set the page size manually 
# (in millimeters), for example for A3 paper size:
# paper = Paper(297, 420)

# An example of how to set the paper margins (in millimeters).
paper.margins = Margins(5, 5, 5, 5)

# Set to PaperOrientation.PORTRAIT if needed.
paper.orientation = PaperOrientation.LANDSCAPE

# How much of each page should overlap with adjacent pages?
# 0: no overlaps
# 0.25: 1/4 of the page overlaps (on each side!), this is the maximum allowed value
page_overlap_factor = 0.1

# Execute the page breaks mechanism.
try:
    if not bounds.break_into_pages(paper, map_scale, page_overlap_factor, page_breaks_filter, export_page):
         raise AssertionError("There are too many pages in the walking papers export. You can increase the limit in WalkingPapers.py script.")
finally:    
    # And then remove the page ID map decoration.
    remove_page_id_decoration()
