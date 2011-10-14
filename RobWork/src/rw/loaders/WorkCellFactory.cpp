/********************************************************************************
 * Copyright 2009 The Robotics Group, The Maersk Mc-Kinney Moller Institute, 
 * Faculty of Engineering, University of Southern Denmark 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ********************************************************************************/


#include "WorkCellLoader.hpp"

#include <rw/loaders/tul/TULLoader.hpp>
#include <rw/loaders/rwxml/XMLRWLoader.hpp>
#include <rw/common/StringUtil.hpp>

#include <rw/plugin/PluginRepository.hpp>
#include <rw/plugin/PluginFactory.hpp>
#include <rw/RobWork.hpp>


using namespace rw::loaders;
using namespace rw::models;
using namespace rw::common;

namespace {

    WorkCell::Ptr loadFromPlugin(const std::string& file, rw::graphics::WorkCellScene::Ptr wcscene){
        PluginRepository &prep = RobWork::getInstance()->getPluginRepository();
        std::vector<PluginFactory<WorkCellLoader>::Ptr> loaderPlugins = prep.getPlugins<WorkCellLoader>();
        BOOST_FOREACH(PluginFactory<WorkCellLoader>::Ptr factory, loaderPlugins){
            //std::cout << "PLUGIN: " << factory->identifier() << std::endl;
            WorkCellLoader::Ptr loader = factory->make();
            // TODO: an image loader or factory should be able to tell what formats it supports
            // perhaps a propertymap on the factory interface could be used
            try {
                if(wcscene!=NULL)
                    loader->setScene(wcscene);
                WorkCell::Ptr wc = loader->loadWorkCell( file );
                return wc;
            } catch (...){
                Log::debugLog() << "Tried loading workcell with plugin: " << StringUtil::quote(factory->identifier()) << " but failed!\n";
                continue;
            }
        }
    };

}

WorkCell::Ptr WorkCellLoader::load(const std::string& file)
{
    const std::string ext = StringUtil::getFileExtension(file);
	if (ext == ".wu" || ext == ".wc" || ext == ".tag" || ext == ".dev") {
        return TULLoader::load(file);
	} else {
	    return XMLRWLoader::loadWorkCell(file);
	}
}

WorkCell::Ptr WorkCellLoader::load(const std::string& file, rw::graphics::WorkCellScene::Ptr wcscene)
{
    const std::string ext = StringUtil::getFileExtension(file);
    if (ext == ".wu" || ext == ".wc" || ext == ".tag" || ext == ".dev") {
        return TULLoader::load(file);
    }
    else {
        return XMLRWLoader::loadWorkCell(file, wcscene);
    }
}