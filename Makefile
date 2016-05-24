target1:
	(pwd;cp map.osm osm4routing-MyParser/bdir;pwd)
	(pwd;cd osm4routing-MyParser;make -f Makefile.osmrouting;pwd)
	(pwd;cd osm4routing-MyParser;mv ./osm4routing ./bdir;pwd)
	#(pwd;cd osm4routing-MyParser;mv ./edges.csv ./bdir;pwd)
	#(pwd;cd osm4routing-MyParser;mv ./nodes.csv ./bdir;pwd)
	(pwd;cd osm4routing-MyParser/bdir;./osm4routing ../../map.osm;pwd)
	(pwd;cd osm4routing-MyParser/bdir;rm worldobjects.csv;pwd)
	(pwd;cd osm4routing-MyParser/bdir;rm temp_ways;pwd)
	(pwd;cd osm4routing-MyParser/boost_1_57_0/libs/graph/example;make -f Makefile.graphevacualtion;pwd)
	(pwd;cd osm4routing-MyParser/bdir;./routing;pwd)
	
	
	
clean:
	(pwd;cd osm4routing-MyParser;rm bdir/osm4routing;pwd)
	(pwd;cd osm4routing-MyParser;rm bdir/routing;pwd)
	(pwd;cd osm4routing-MyParser;rm bdir/osmrender.osm;pwd)
	(pwd;cd osm4routing-MyParser;rm bdir/map.osm;pwd)
	(pwd;cd osm4routing-MyParser;rm bdir/edges.csv;pwd)
	(pwd;cd osm4routing-MyParser;rm bdir/nodes.csv;pwd)
