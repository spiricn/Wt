#ifndef WT_REMOTEFILESYSTEMPROTOCOL_H
#define WT_REMOTEFILESYSTEMPROTOCOL_H

namespace wt
{

namespace rfsp
{

enum OperationCode{
	eCODE_INVALID = -1,

	eCODE_STREAM_OPEN,
	eCODE_READ

}; // </OperationCode>

}

} // </wt>

#endif // </WT_REMOTEFILESYSTEMPROTOCOL_H>
