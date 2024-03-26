#include"init.h"
#include"../yaml-cpp_063/include/yaml-cpp/yaml.h"
#include"../log.h"

namespace YSSLM {
	namespace engine {
		namespace init {
InitBase Init::init_base_;

bool InitBase::Init_() {
		
	auto node = YAML::LoadFile(firing_path_);
	
	if (!node["Component"].IsDefined()) {
		ULOG_ERROR("InitBase::Compenent_::Initializa():: YAML Component fell !");
		return false;
	}

	for (auto data : node["Component"]) {
		if (!data["Open"].IsDefined() || node["Open"].IsNull() || !data["Open"].as<bool>()) {
			continue;
		}
		if (!data["Class"].IsDefined() || node["Class"].IsNull()) {
			ULOG_ERROR("InitBase::Compenent_::Initializa():: YAML Class fell !");
			return false;
		}
		if (!data["Name"].IsDefined() || node["Name"].IsNull()) {
			ULOG_ERROR("InitBase::Compenent_::Initializa():: YAML Name fell !");
			return false;
		}
		auto name = data["Name"].as<std::string>();
		if(Compenent_.find(name) != Compenent_.end()){
			ULOG_ERROR("InitBase::Compenent_::Initializa():: YAML Name(%d) is exist , fell !");
			return false;
		}
		Compenent_.emplace(name , factoryComponent::produce(data["Class"].as<std::string>()));
		if (!Compenent_[name]->yaml_init(data, firing_path_)) {
			ULOG_ERROR("InitBase::Compenent_::yaml_init(%s) fell !" , data["Class"].as<std::string>().c_str());
			return false;
		}
		if (!Compenent_[name]->Init()) {
			ULOG_ERROR("InitBase::Compenent_::Init(%s | %s) fell !" , data["Class"].as<std::string>().c_str() , name.c_str());
			return false;
		}
		ULOG_INFO("InitBase::Compenent_::Init(%s | %s) succeed !" , data["Class"].as<std::string>().c_str() , name.c_str());
	}

	ULOG_INFO("InitBase::Compenent_::Initializa() succeed !");

	for (auto data : Compenent_) {
		if (!data.second->write_init()) {
			ULOG_ERROR("InitBase::Compenent_::write_init() fell !");
			return false;
		}
	}

	ULOG_INFO("InitBase::write_init::Initializa() succeed !");
	
	if (!msg_c::MesaageCore::Udp_init()) {
		ULOG_ERROR("InitBase::Udp_msg::Udp_init() fell !");
		return false;
	}

	ULOG_INFO("InitBase::Udp_msg::Initializa() succeed !");

	return true;
}

bool InitBase::Close_() {

	msg_c::MesaageCore::Udp_close();

	ULOG_INFO("InitBase::Udp_msg::DeInitializa() succeed !");
	
	for (auto data : Compenent_) {
		data.second->write_close();
	}
	
	ULOG_INFO("InitBase::write_close::DeInitializa() succeed !");

	for (auto data : Compenent_) {
		
		ULOG_INFO("InitBase::Compenent_::DeInitializa(%s) start !", data.second->node_name().c_str());

		if (!data.second->Close()) {
			ULOG_ERROR("InitBase::Compenent_::DeInitializa(%s) fell !", data.second->node_name().c_str());
			return false;
		}
		ULOG_INFO("InitBase::Compenent_::DeInitializa(%s) succeed !", data.second->node_name().c_str());
		data.second = nullptr;
	}

	ULOG_INFO("InitBase::Compenent_::DeInitializa() succeed !");

	return true;
}

bool Init::init(int argc, char **argv){
	std::string startTimes;
	if(argc < 2){
		ULOG_ERROR("Init::init() argc < 2 fell !");
		return false;
	}
	init_base_.init(argv[1]);
	return init_base_.Init_();
}


		}
	}
}
