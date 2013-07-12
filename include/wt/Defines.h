#ifndef WT_DEFINES_H
#define WT_DEFINES_H


#ifndef WT_DISALLOW_COPY
#define WT_DISALLOW_COPY(class_name) \
	class_name(const class_name& obj); \
	class_name& operator=(const class_name& obj);
#endif

#define WT_UNUSED(x) (void)(x)

#endif
