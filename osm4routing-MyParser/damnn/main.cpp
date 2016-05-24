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
    << tag("sample-tag") // root tag

      << tag("some-tag") // child tag
        << attr("int-attribute") << 123
        << attr("double-attribute") << 456.789
        << chardata() << "This is the text"
      << endtag() // close current tag

      << tag("empty-self-closed-tag") << endtag() // sibling of <some-tag>

      << tag() << "computed-name-tag" << attr("text-attr") << "a bit of text"
      << endtag()

      << tag("deep-tag") // deep enclosing
        << tag("sub-tag-2")
          << tag("sub-tag-3")
      << endtag("deep-tag"); // close all tags up to specified

  // look: I didn't close "sample-tag", it will be closed in XmlStream destructor

  return 0;
}