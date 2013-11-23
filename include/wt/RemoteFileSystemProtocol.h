#ifndef WT_REMOTEFILESYSTEMPROTOCOL_H
#define WT_REMOTEFILESYSTEMPROTOCOL_H

namespace wt
{

namespace rfsp
{

enum OperationCode{
	eCODE_INVALID = -1,

	eCODE_STREAM_OPEN = 0,
	eCODE_READ,
	eCODE_WRITE,
	eCODE_SEEK,
	eCODE_TELL,
	eCODE_IS_OPEN,
	eCODE_IS_SEEKABLE,
	eCODE_GET_SIZE,

	eCODE_MAX
}; // </OperationCode>

}

} // </wt>

#endif // </WT_REMOTEFILESYSTEMPROTOCOL_H>
