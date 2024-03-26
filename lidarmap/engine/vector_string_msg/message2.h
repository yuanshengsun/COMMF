#ifndef _YuanshengSun_LM_Engine_VectorStringMsg_Message2_H_
#define _YuanshengSun_LM_Engine_VectorStringMsg_Message2_H_
#include <vector>
#include <string>
#include <memory>
#include <cstring>
namespace YSSLM {
namespace engine {
namespace vector_string_msg {


struct StringMsg {

public:

	std::string DATA;

	StringMsg(){}

	StringMsg(const std::string& data):DATA(data){}

	void  operator=(const StringMsg &data){
		DATA = data.DATA;
	}

	void Set(const std::shared_ptr<const std::string>& data){
		DATA = std::move(*data);
	}
	std::shared_ptr<std::string> Get(){
		return std::make_shared<std::string>(std::move(DATA));
	}

};
}
}
}

#endif