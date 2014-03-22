#ifndef WT_OWNPTR_H
#define WT_OWNPTR_H

namespace wt
{

template<class Type>
class OwnPtr{
public:
	OwnPtr(Type* ref=NULL) : mRef(ref){
	}

	~OwnPtr(){
		if(mRef){
			delete mRef;
		}
	}

	OwnPtr& operator=(Type* ref){
		mRef = ref;
		return *this;
	}

	void set(Type* ref){
		if(mRef){
			delete mRef;
		}

		mRef = ref;
	}

	void release(){
		mRef = ref;
	}

	bool operator==(Type* ref) const{
		return mRef == ref;
	}

	bool operator!=(Type* ref){
		return !this->operator==(ref);
	}

	Type& operator*(){
		return *mRef;
	}

	Type* operator->(){
		return mRef;
	}

	Type* get() const{
		return mRef;
	}

private:
	OwnPtr(const OwnPtr& other);

	Type* mRef;
}; // </OwnPtr>

} // </wt>

#endif // </WT_OWNPTR_H>
