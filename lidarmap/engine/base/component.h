#ifndef _YuanshengSun_LM_Engine_Base_Component_H_
#define _YuanshengSun_LM_Engine_Base_Component_H_

#include"../message_core/write.h"
#include"../message_core/read.h"


namespace YSSLM {
	namespace engine {
		namespace base {
			
			using CWrite = YSSLM::engine::msg_c::CWrite;
			using CRead = YSSLM::engine::msg_c::CRead;
			using Write = YSSLM::engine::msg_c::Write;
			using Read = YSSLM::engine::msg_c::Read;
			using MesaageCore = YSSLM::engine::msg_c::MesaageCore;

class NodeComponent {

public:

	std::string config_file() { return Component_config_path; }

	std::string node_name() { return Component_name; }

	template<class T>
	std::shared_ptr<CWrite> NodeWrite(
						const std::string chenlne, 
						int policy = _YSSLM_THREADATTR_INIT_policy_,
						int sched_priority = _YSSLM_THREADATTR_INIT_sched_priority_,
						int cpu_id = _YSSLM_THREADATTR_INIT_cpu_id_
					) {

				std::shared_ptr<std::vector<std::shared_ptr<Write>>> data = 
					std::make_shared<std::vector<std::shared_ptr<Write>>>();
				if (Write_chennle_name.find(chenlne) == Write_chennle_name.end()) {
					return nullptr;
				}
				for(auto &data2 : Write_chennle_name[chenlne]){
					data->push_back(std::make_shared<Write>(data2));
					Write_vector.push_back(data->back());
					Write_id.push_back(MesaageCore::write_add(data2.Channel));
					Write_name.push_back(typeid(T).name());

					add_Write_udp_addr_port_map(data2.udp_addr , data2.udp_port , data2.udp_open);
				}
				return std::make_shared<CWrite>(data, policy , sched_priority, cpu_id);
			}

	template<class T>
	std::shared_ptr<CRead> NodeRead(
						const std::string chenlne, 
						std::function<void(std::shared_ptr<T>&)> call_back, 
						bool ptr_read = true,
						int num_thr = 1,
						int policy = _YSSLM_THREADATTR_INIT_policy_,
						int sched_priority = _YSSLM_THREADATTR_INIT_sched_priority_,
						int cpu_id = _YSSLM_THREADATTR_INIT_cpu_id_
					) {

				std::function<void(std::shared_ptr<std::string>&)> call_back_s = [call_back,ptr_read](std::shared_ptr<std::string>& msg_s){
					std::shared_ptr<T> msg = std::make_shared<T>();
					msg->Set(std::shared_ptr<const std::string>(msg_s));
					call_back(msg);
				};
				std::shared_ptr<std::vector<std::shared_ptr<YSSLM::engine::msg_c::Reades<T>>>> read_T_data =
					std::make_shared<std::vector<std::shared_ptr<YSSLM::engine::msg_c::Reades<T>>>>();
				if (Read_chennle_name.find(chenlne) == Read_chennle_name.end()) {
					return nullptr;
				}
				for(auto &data2 : Read_chennle_name[chenlne]){
					read_T_data->push_back(std::make_shared<YSSLM::engine::msg_c::Reades<T>>());
					read_T_data->back()->call_back = call_back;
					read_T_data->back()->ptr_read = ptr_read;
					read_T_data->back()->num_thr = num_thr;
					read_T_data->back()->policy = policy;
					read_T_data->back()->sched_priority = sched_priority;
					read_T_data->back()->cpu_id = cpu_id;
					MesaageCore::read_add(data2.Channel, typeid(T).name(), read_T_data->back());
					MesaageCore::udp_read_add(data2.Channel, data2 , call_back_s, policy ,sched_priority , cpu_id , num_thr);
				}
				return std::make_shared<CRead>(read_T_data);
			}

	virtual bool Init() = 0;

	virtual bool Close() = 0;

	bool write_init() ;

	void write_close() ;
	
	bool yaml_init(YAML::Node node,std::string Component_config_path_back) ;

private:
	using ChannelData = YSSLM::engine::msg_c::ChannelData;

	std::vector<std::shared_ptr<Write>> Write_vector;
	std::vector<uint64_t> Write_id;
	std::vector<std::string> Write_name;
	std::string Component_name;
	std::map<std::string, std::vector<ChannelData>> Read_chennle_name;
	std::map<std::string, std::vector<ChannelData>> Write_chennle_name;
	std::string Component_config_path;

	static void add_Write_udp_addr_port_map(int32_t addr, int port , bool open);
	static std::map<uint32_t , std::map<int , bool>> Write_udp_addr_port_map_;

public:

	static bool write_udp_is_exist(int32_t addr, int port);
};

		}
	}
}

#endif
