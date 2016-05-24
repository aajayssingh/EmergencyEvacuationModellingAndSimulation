//////////////////////////////////////////////////////////////////////
//
// Example of using XmlStream class.
// 
// Author: Oboltus, December 2003
//
// This code is provided "as is", with absolutely no warranty expressed
// or implied. Any use is at your own risk.
//
//////////////////////////////////////////////////////////////////////

#include <fstream>
#include "xmlwriter.h"

using namespace std;
using namespace	xmlw;





int main()
{
  ofstream f("sample.xml");
  XmlStream xml(f);

  xml << prolog() // write XML file declaration
    << tag("CityModel") // root tag
	<<tag("cityObjectMember")

		      << tag("bldg:Building") // child tag
			<< attr("gml:id") << "id alphanumeric attribute val"
			<< tag("gml:name")<<chardata()<<"Example building L0D0"
			<< endtag() // close current tag

		      << tag("bldg:measuredHeight")<<attr("uom")<<"#m" <<10<< endtag() // sibling of <some-tag>

		     

		      << tag("gml:point") // deep enclosing
			<<attr("gml:pos")<<1.00000<<" "<<2.0000
			<<endtag("gml:pos")
		      << endtag("gml:point"); // close all tags up to specified

  // look: I didn't close "sample-tag", it will be closed in XmlStream destructor

  return 0;
}
