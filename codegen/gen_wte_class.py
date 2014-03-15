className = CG_getArg(0)


parentClass = None

if CG_getNumArgs() >= 2:
   parentClass = CG_getArg(1)

headerGuard = 'WTE_%s_H' % className.upper()

###############################################################################
headerTemplate = '''\
#ifndef <%= headerGuard %>
#define <%= headerGuard %>

class <%= className %><%= ' : public %s' % parentClass if parentClass else '' %>{
public:
	<%= className %>();

	virtual ~<%= className %>();

private:
}; // </<%= className %>>


#endif // </<%= headerGuard %>>
'''

CG_convertSource(headerTemplate, '%s.h' % className)

###############################################################################

sourceTemplate = '''\
#include "wte/stdafx.h"
#include "wte/<%= className %>.h"

#define TD_TRACE_TAG "<%= className %>"

<%= className %>::<%= className %>()<%= ' : %s() ' % parentClass if parentClass else '' %>{
}

<%= className %>::~<%= className %>(){
}
'''

CG_convertSource(sourceTemplate, '%s.cpp' % className)

###############################################################################