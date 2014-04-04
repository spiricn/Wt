className = CG_getArg(0)

headerGuard = 'WT_%s_H' % className.upper()

###############################################################################


headerFileName =  '%s.h' % className

headerTemplate = '''\
/**
 * @file <%= headerFileName %>
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef <%= headerGuard %>
#define <%= headerGuard %>

namespace wt
{

class <%= className %>{
public:
	<%= className %>();

	~<%= className %>();

private:
}; // </<%= className %>>

} // </wt>

#endif // </<%= headerGuard %>>
'''

CG_convertSource(headerTemplate, headerFileName)

###############################################################################

cppFileName = '%s.cpp' % className

sourceTemplate = '''\
/**
 * @file <%= cppFileName %>
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/<%= className %>.h"

#define TD_TRACE_TAG "<%= className %>"

namespace wt
{

<%= className %>::<%= className %>(){
}

<%= className %>::~<%= className %>(){
}
	
} // </wt>

'''

CG_convertSource(sourceTemplate, cppFileName)

###############################################################################