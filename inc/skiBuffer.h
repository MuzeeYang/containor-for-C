#ifndef __SKI_BUFFER_H__
#define __SKI_BUFFER_H__

#include "stdlib.h"
#include "skiTypes.h"

#define SKIBUFFER_IDX_END	(-1)

/**
 * breif: create a buffer
 * Para1: pointer of a buf to init, if NULL init buffer with 0
 * Para2: length of buf, or size of buffer to create if para1 is NULL.
	if length less than 16, length would be set to 16.
 * return: handler of buffer
 */
skiHandler_t skiBuffer_create(void* buf, size_t length);

/**
 * breif: destroy a buffer
 * Para1: handler of buffer to be operated
 * return: void
 */
void skiBuffer_destroy(skiHandler_t handler);

/**
 * breif: copy a buffer
 * Para1: handler of buffer to be operated
 * return: handler of new a buffer
 */
skiHandler_t skiBuffer_copy(skiHandler_t handler);

/**
 * breif: resize a buffer
 * Para1: handler of buffer to be operated
 * Para2: new size of buffer
 * return: size of buffer currently on success, 0 if error
 * ps: this function could increase buffer or truncate it with setting 0
 */
size_t skiBuffer_resize(skiHandler_t handler, size_t size);

/**
 * breif: insert a buf into buffer on any position
 * Para1: handler of buffer to be operated
 * Para2: position to insert. if offset is larger than size, it would be set to size.
 * Para3: pointer of buf
 * Para4: length of buf
 * return: inserted size on success, 0 if error
 * ps: offset could be size of buffer, that means insert buf append buffer
	SKIBUFFER_IDX_END would help.
 */
size_t skiBuffer_insert(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);

/**
 * breif: insert a buf into buffer on tail
 * Para1: handler of buffer to be operated
 * Para2: pointer of buf
 * Para3: length of buf
 * return: inserted size on success, 0 if error
 * ps: this function is like a special skiBuffer_insert()
 */
size_t skiBuffer_append(skiHandler_t handler, void* buf, size_t length);

/**
 * breif: cut a buf from buffer on any position
 * Para1: handler of buffer to be operated
 * Para2: position to start cutting. if offset is larger than size, it would be set to size.
 * Para3: pointer of buf(out), data cutted would be dropped if it was NULL.
 * Para4: length of buf to cut
 * return: cutted size on success, 0 if error
 * ps: cutted length might be less than para4, it would be dicided on the rest of buffer
 */
size_t skiBuffer_cut(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);

/**
 * breif: cut a buf from buffer on tail
 * Para1: handler of buffer to be operated
 * Para2: pointer of buf(out), data cutted would be dropped if it was NULL.
 * Para3: length of buf to cut
 * return: cutted size on success, 0 if error
 * ps: this function is like a special skiBuffer_cut()
 */
size_t skiBuffer_truncate(skiHandler_t handler, void* buf, size_t length);

/**
 * breif: find whether buffer has buf
 * Para1: handler of buffer to be operated
 * Para2: position to start cutting. if offset is larger than size, it would be set to size.
 * Para3: pointer of buf to search
 * Para4: length of buf
 * return: index of buffer on success, size of buffer if error
 * ps: index must be less than size, this is a difference against offset
 */
skiIndex_t skiBuffer_find(skiHandler_t handler, skiIndex_t offset, void* buf, size_t length);

/**
 * breif: replace a buf on buffer
 * Para1: handler of buffer to be operated
 * Para2: position to start replacing. if offset is larger than size, it would be set to size.
 * Para3: length of data to delete
 * Para4: pointer of buf to update
 * Para5: length of buf to update
 * return: updated size on success, 0 if error
 */
size_t skiBuffer_replace(skiHandler_t handler, skiIndex_t offset, size_t rpLength, void* buf, size_t bufLength);
/**
 * breif: get data of buffer by index
 * Para1: handler of buffer to be operated
 * Para2: index of buffer, must be less than size
 * return: pointer of data in buffer on success, NULL if error
 */
char* skiBuffer_at(skiHandler_t handler, skiIndex_t idx);

/**
 * breif: get size of buffer
 * Para1: handler of buffer to be operated
 * return: size of buffer on success, 0 if error
 */
size_t skiBuffer_size(skiHandler_t handler);
#endif
