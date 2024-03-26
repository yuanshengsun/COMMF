#ifndef _YuanshengSun_LM_Engine_VectorStringMsg_Message_H_
#define _YuanshengSun_LM_Engine_VectorStringMsg_Message_H_
#include <vector>
#include <string>
#include <memory>
#include <cstring>
namespace YSSLM {
namespace engine {
namespace vector_string_msg {


struct VectorStringMsg {

public:

	std::vector<std::string> DATA;

	VectorStringMsg(){}

	VectorStringMsg(const std::string& data){
		DATA.emplace_back(data);
	}

	void  operator=(const VectorStringMsg &data){
		DATA = data.DATA;
	}

	void Set(const std::shared_ptr<const std::string>& data){
		uint16_t nu;
		char* data_m = (char*)data->c_str();
		memcpy(((char*)&nu), data_m , sizeof(nu));
		data_m += sizeof(nu);

		for(int i = 0 ; i < nu; ++i ){
			uint16_t num;
			memcpy(((char*)&num), data_m , sizeof(num));
			data_m += sizeof(num);

			DATA.emplace_back();
			DATA.back().resize(num);
			char* data_s = (char*)DATA.back().c_str();
			memcpy(data_s, data_m , num);
			data_m += num;
		}
	}
	std::shared_ptr<std::string> Get(){
		std::shared_ptr<std::string>data = std::make_shared<std::string>();

		uint16_t nu = (uint16_t)DATA.size();
		int len = 0;
		data->resize(sizeof(nu));
		memcpy((char*)data->c_str() + len , ((char*)&nu), sizeof(nu));
		len+= sizeof(nu);

		for(int i = 0 ; i < (int)DATA.size(); ++i ){
			nu = (uint16_t)DATA[i].size();
			data->resize(len + sizeof(nu));
			memcpy((char*)data->c_str() + len , ((char*)&nu), sizeof(nu));
			len+= sizeof(nu);
			
			
			data->resize(len + (int)DATA[i].size());
			memcpy((char*)data->c_str() + len , DATA[i].c_str() , DATA[i].size());
			len+= (int)DATA[i].size();
		}
		
		return data;
	}


private:


};
}
}
}

#endif