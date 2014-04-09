eventName = CG_getArg(0)

className = eventName + "Event"

template = '''\
class <%= className %> : public AEvent{
public:

public:
	<%= className %>(){
	}

	const EventType& getType() const{
		return TYPE;
	}
	
private:
	void serialize(LuaObject& dst){
		AEvent::serialize(dst);
	}

	void deserialize(LuaObject& src){
		AEvent::deserialize(src);
	}
	
public:
	static const EventType TYPE;
	
}; // </<%= className %>>

const EventType <%= className %>::TYPE = "<%= className %>";

'''

CG_convertSource(template, '%s.cpp' % className)