#ifndef _YuanshengSun_LM_Engine_Factory_FactoryComponent_H_
#define _YuanshengSun_LM_Engine_Factory_FactoryComponent_H_

#include <unordered_map>
#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include"../base/component.h"

namespace YSSLM {
	namespace engine {
		namespace factory {

using NodeComponent = YSSLM::engine::base::NodeComponent;

struct factoryComponent {
	template<typename T>
	struct register_t
	{
		register_t(const std::string& keys)
		{
			std::string key = typeid(T).name();

			factoryComponent::get().map_component.emplace(key, [] { return std::make_shared<T>(); });
			printf(" \033[33m==>\033[0m \033[32mfactoryComponent 1 de: [%s]\033[0m\n", key.c_str());
		}

		template<typename... Args>
		register_t(const std::string& keys, Args... args)
		{
			std::string key = typeid(T).name();

			factoryComponent::get().map_component.emplace(key, [=] { return std::make_shared<T>(args...); });
			printf(" \033[33m==>\033[0m \033[32mfactoryComponent 2 de: [%s]\033[0m\n", key.c_str());
		}
	};

	static std::shared_ptr<NodeComponent> produce(const std::string& keys)
	{
		std::vector<std::string> keysv;
		keysv.emplace_back();
		for(int i = 0 ; i < (int)keys.size(); ++i){
			if(keys[i] != ':'){
				keysv.back().push_back(keys[i]);
			}
			else{
				++i;
				keysv.emplace_back();
			}
		}
		
		std::string key = "N";
		for(int i  = 0 ; i < (int)keysv.size(); ++i){
			key += std::to_string((int)keysv[i].size()) + keysv[i];
		}
		key += "E";
		
		if (map_component.find(key) == map_component.end()){
			std::string keyss = "the component key [" + key + "] is not exist!";
			throw std::invalid_argument(keyss.c_str());
		}

		return map_component[key]();
	}
private:
	factoryComponent() = default;
	factoryComponent(const factoryComponent&) = delete;
	factoryComponent(factoryComponent&&) = delete;

	static factoryComponent& get()
	{
		static factoryComponent instance;
		return instance;
	}

	static std::unordered_map<std::string, std::function<std::shared_ptr<NodeComponent>()>> map_component;
};



		}
	}
}

#define TRANS(name) #name
#define REGISTER_COMPONENT_VNAME(T) reg_msg_##T##_
#define REGISTER_COMPONENT(T,...) static YSSLM::engine::factory::factoryComponent::register_t<T> REGISTER_COMPONENT_VNAME(T)(TRANS(T), ##__VA_ARGS__);

#endif