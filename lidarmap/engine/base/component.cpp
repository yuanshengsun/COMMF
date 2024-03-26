#include"component.h"
#include"../message_core/message_core.h"
#include"../yaml-cpp_063/include/yaml-cpp/yaml.h"
#include"../log.h"

namespace YSSLM {
	namespace engine {
		namespace base {
	
	std::map<uint32_t , std::map<int , bool>> NodeComponent::Write_udp_addr_port_map_;
	void NodeComponent::add_Write_udp_addr_port_map(int32_t addr, int port, bool open){
		if(!open){
			return;
		}
		if(Write_udp_addr_port_map_.find(addr) == Write_udp_addr_port_map_.end()){
			Write_udp_addr_port_map_.emplace(addr , std::map<int , bool>());
		}
		if(Write_udp_addr_port_map_[addr].find(port) == Write_udp_addr_port_map_[addr].end()){
			Write_udp_addr_port_map_[addr].emplace(port , true);
		}
	}
	bool NodeComponent::write_udp_is_exist(int32_t addr, int port){
		if(Write_udp_addr_port_map_.find(addr) == Write_udp_addr_port_map_.end()){
			return false;
		}
		if(Write_udp_addr_port_map_[addr].find(port) == Write_udp_addr_port_map_[addr].end()){
			return false;
		}
		return true;
	}

	bool NodeComponent::write_init() {
		for (int i = 0; i < (int)Write_id.size(); ++i) {

			if (!Write_vector[i]->Write_init(MesaageCore::load_init(Write_name[i], Write_id[i]))) {
				ULOG_ERROR("NodeComponent::write_init() [%d]Write_init fell !" , i);
				return false;
			}
		}

		Write_id.clear();
		Write_name.clear();
		return true;
	}
	void NodeComponent::write_close() {

		for (auto & data : Write_vector) {
			data->close_flash();
		}
		Write_vector.clear();
	}

	bool NodeComponent::yaml_init(YAML::Node node,std::string Component_config_path_back) {
		if (!node["Name"].IsDefined() || node["Name"].IsNull()) {
			ULOG_ERROR("NodeComponent::yaml_init() YAML Name fell !");
			return false;
		}
		Component_name = node["Name"].as<std::string>();

		if (!node["ConfigFile"].IsDefined()) {
			ULOG_ERROR("NodeComponent::yaml_init() YAML ConfigFile fell !");
			return false;
		}

		for (int i = Component_config_path_back.size() - 1; i >= 0; --i) {
			if (Component_config_path_back[i] == '/' || Component_config_path_back[i] == '\\') {
				break;
			}
			Component_config_path_back.pop_back();
		}
		Component_config_path = Component_config_path_back+ node["ConfigFile"].as<std::string>();

		if (!node["Read"].IsDefined()) {
			ULOG_ERROR("NodeComponent::yaml_init() YAML Read fell !");
			return false;
		}
		for (auto data : node["Read"]) {
			if (Read_chennle_name.find(data.first.as<std::string>()) != Read_chennle_name.end()) {
				ULOG_ERROR("NodeComponent::yaml_init() YAML Read[%s] fell !" , data.first.as<std::string>().c_str());
				return false;
			}
			Read_chennle_name.insert(std::pair<std::string, std::vector<ChannelData>>
				(data.first.as<std::string>(), std::vector<ChannelData>()));
			for (auto data2 : data.second) {
				if (!data2["Channel"].IsDefined()) {
					ULOG_ERROR("NodeComponent::yaml_init() YAML Read[%s][Channel] fell !" , data.first.as<std::string>().c_str());
					return false;
				}
				if (!data2["UDP_AddrPort"].IsDefined()) {
					ULOG_ERROR("NodeComponent::yaml_init() YAML Read[%s][UDP_AddrPort] fell !" , data.first.as<std::string>().c_str());
					return false;
				}
				auto udp_s = data2["UDP_AddrPort"].as<std::string>();
				if(data2["UDP_AddrPort"].IsNull()){
					Read_chennle_name[data.first.as<std::string>()].emplace_back(
						data2["Channel"].as<std::string>(),
						false,
						0,
						0,
						""
					);
				}
				else{
					int udp_s_i = 0;
					for( ; udp_s_i< (int)udp_s.size() ; ++udp_s_i ){
						if(udp_s[udp_s_i] == ':'){
							break;
						}
					}
					if(udp_s_i==0 || udp_s_i == (int)udp_s.size()){
						ULOG_ERROR("NodeComponent::yaml_init() YAML Read[%s][UDP_AddrPort] fell !" , data.first.as<std::string>().c_str());
						return false;
					}
					std::string udp_addr_s(std::begin(udp_s) + 0 ,std::begin(udp_s) + udp_s_i);
					uint32_t udp_addr = (udp_addr_s == "0")? 0 : inet_addr(udp_addr_s.c_str());
					std::string udp_port_s(std::begin(udp_s) + udp_s_i +1, std::end(udp_s));
					int udp_port = atoi(udp_port_s.c_str());

					Read_chennle_name[data.first.as<std::string>()].emplace_back(
						data2["Channel"].as<std::string>(),
						true,
						udp_addr,
						udp_port,
						udp_addr_s
					);
				}
			}
		}

		if (!node["Write"].IsDefined()) {
			ULOG_ERROR("NodeComponent::yaml_init() YAML Write fell !");
			return false;
		}
		for (auto data : node["Write"]) {
			if (Write_chennle_name.find(data.first.as<std::string>()) != Write_chennle_name.end()) {
				ULOG_ERROR("NodeComponent::yaml_init() YAML Write[%s] fell !" , data.first.as<std::string>().c_str());
				return false;
			}
			Write_chennle_name.insert(std::pair<std::string, std::vector<ChannelData>>
				(data.first.as<std::string>(), std::vector<ChannelData>()));
			for (auto data2 : data.second) {
				if (!data2["Channel"].IsDefined()) {
					ULOG_ERROR("NodeComponent::yaml_init() YAML Write[%s][Channel] fell !" , data.first.as<std::string>().c_str());
					return false;
				}
				if (!data2["UDP_AddrPort"].IsDefined()) {
					ULOG_ERROR("NodeComponent::yaml_init() YAML Write[%s][UDP_AddrPort] fell !" , data.first.as<std::string>().c_str());
					return false;
				}
				auto udp_s = data2["UDP_AddrPort"].as<std::string>();
				if(data2["UDP_AddrPort"].IsNull()){
					Write_chennle_name[data.first.as<std::string>()].emplace_back(
						data2["Channel"].as<std::string>(),
						false,
						0,
						0,
						""
					);
				}
				else{
					int udp_s_i = 0;
					for( ; udp_s_i< (int)udp_s.size() ; ++udp_s_i ){
						if(udp_s[udp_s_i] == ':'){
							break;
						}
					}
					if(udp_s_i==0 || udp_s_i == (int)udp_s.size()){
						ULOG_ERROR("NodeComponent::yaml_init() YAML Write[%s][UDP_AddrPort] fell !" , data.first.as<std::string>().c_str());
						return false;
					}
					std::string udp_addr_s(std::begin(udp_s) + 0 ,std::begin(udp_s) + udp_s_i);
					uint32_t udp_addr = (udp_addr_s == "0")? 0 : inet_addr(udp_addr_s.c_str());
					std::string udp_port_s(std::begin(udp_s) + udp_s_i +1 , std::end(udp_s));
					int udp_port = atoi(udp_port_s.c_str());

					Write_chennle_name[data.first.as<std::string>()].emplace_back(
						data2["Channel"].as<std::string>(),
						true,
						udp_addr,
						udp_port,
						udp_addr_s
					);
				}
			}
		}

		return true;
	}

		}
	}
}

