className = CG_getArg(0)

headerGuard = 'WT_%s_H' % className.upper()

###############################################################################

headerTemplate = '''\
#ifndef <%= headerGuard %>
#define <%= headerGuard %>

namespace wt
{

class <%= className %>{
public:
	<%= className %>();

	virtual ~<%= className %>();

private:
}; // </<%= className %>>

} // </wt>

#endif // </<%= headerGuard %>>
'''

CG_convertSource(headerTemplate, '%s.h' % className)

###############################################################################

sourceTemplate = '''\
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

CG_convertSource(sourceTemplate, '%s.cpp' % className)

###############################################################################