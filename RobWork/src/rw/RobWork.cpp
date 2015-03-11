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
#include "RobWork.hpp"

#include <rw/common/Ptr.hpp>
#include <rw/common/os.hpp>
#include <boost/filesystem.hpp>
#include <rw/loaders/xml/XMLPropertyLoader.hpp>
#include <rw/loaders/xml/XMLPropertySaver.hpp>
#include <rw/common/ExtensionRegistry.hpp>
#include <rw/common/Plugin.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/parsers.hpp>

#include <cstdlib>
using namespace rw;
using namespace rw::common;
using namespace rw::loaders;
//Using the below does not work with MSVS 2010 and MSVS 2012
//using namespace boost::filesystem;
using boost::filesystem::path;
using boost::filesystem::exists;
using boost::filesystem::initial_path;

using namespace boost::program_options;

#if defined(RW_CYGWIN)
    #define RWCFGHOMEDIR std::string(std::getenv("HOME")) + "/.config/robwork/"
    #define RWCFGFILE std::string(std::getenv("HOME")) + "/.config/robwork/robwork-" + RW_BUILD_TYPE + "-"+ RW_VERSION + ".cfg.xml"
#elif defined(RW_WIN32)
    #define RWCFGHOMEDIR std::string(std::getenv("APPDATA")) + "/robwork/"
    #define RWCFGFILE std::string(std::getenv("APPDATA")) + "/robwork/robwork-" + RW_BUILD_TYPE + "-"+ RW_VERSION + ".cfg.xml"
#elif defined(RW_MACOS)
    #define RWCFGHOMEDIR std::string(std::getenv("HOME"))  + "/Library/Preferences/"
    #define RWCFGFILE std::string(std::getenv("HOME")) + "/Library/Preferences/com.robwork-" + RW_BUILD_TYPE + "-"+ RW_VERSION + ".cfg.xml"
#elif defined(RW_LINUX)
    #define RWCFGHOMEDIR std::string(std::getenv("HOME")) + "/.config/robwork/"
    #define RWCFGFILE std::string(std::getenv("HOME")) + "/.config/robwork/robwork-" + RW_BUILD_TYPE + "-" + RW_VERSION + ".cfg.xml"
#endif

RobWork::RobWork(void) 
{
}

RobWork::~RobWork(void)
{
    if(!_settingsFile.empty()) {
        XMLPropertySaver::save( _settings, _settingsFile );
	}
}

void RobWork::initialize(const std::vector<std::string>& plugins){
    Log::infoLog() << "Initializing ROBWORK" << std::endl;

    // user supplied arguments will always be taken into account

	// this is the search priority
    // 1. check if env variables define where robwork and therefore plugins are located
	// 2. search for config file from execution directory
	// 3. search from user home directory (OS specific)
    // 4. check hardcoded build directory
    path ipath = initial_path();
    std::string rwsettingsPath = ipath.string() + "/robwork-" + RW_BUILD_TYPE + "-" + RW_VERSION + ".cfg.xml";
    char* rwRootVar = getenv("RW_ROOT");

    if( rwRootVar != NULL ){
    	Log::infoLog() << "Found RobWork root dir in environment variable RW_ROOT..." << std::endl;
        // create the file in default current location
        PropertyMap plugins;

        plugins.add("location-1","Default plugin location",std::string("plugins/"));

        char* rwRootVar = getenv("RW_ROOT");
        if(rwRootVar != NULL)
            plugins.add("location-2","Default plugin location for RobWork",std::string(rwRootVar)+"/libs/"+RW_BUILD_TYPE+"/");

        char* rwsRootVar = getenv("RWS_ROOT");
        if(rwsRootVar != NULL)
            plugins.add("location-3","Default plugin location for RobWorkStudio",std::string(rwsRootVar)+"/libs/"+RW_BUILD_TYPE+"/");

        char* rwsimRootVar = getenv("RWSIM_ROOT");
        if(rwsimRootVar != NULL)
            plugins.add("location-4","Default plugin location for RobWorkSim",std::string(rwsimRootVar)+"/libs/"+RW_BUILD_TYPE+"/");

        char* rwhwRootVar = getenv("RWHW_ROOT");
        if(rwhwRootVar != NULL)
            plugins.add("location-5","Default plugin location for RobWorkHardware",std::string(rwhwRootVar)+"/libs/"+RW_BUILD_TYPE+"/");

        _settings.add("plugins","List of plugins or plugin locations",plugins);

    } else if( exists(rwsettingsPath) ){
    	Log::infoLog() << "Found robwork configuration file in execution directory..." << std::endl;
        _settings = XMLPropertyLoader::load( rwsettingsPath );
        _settings.add("cfgfile", "", rwsettingsPath );
    } else if( exists( RWCFGFILE ) ){
    	Log::infoLog() << "Found robwork configuration filr in global configuration directory:\n\t" << std::string(RWCFGFILE) << std::endl;
    	rwsettingsPath = std::string(RWCFGFILE);
        _settings = XMLPropertyLoader::load( rwsettingsPath );
    	_settings.add("cfgfile", "", rwsettingsPath );
    } else if( exists( std::string(RW_BUILD_DIR) ) ){
    	// check if the build directory exist
    	Log::infoLog() << "Found robwork in build directory: \"" << std::string(RW_BUILD_DIR) << "\""<< std::endl;
    	std::string buildDir( RW_BUILD_DIR );

        // create the file in default current location
        PropertyMap plugins;
        plugins.add("location-1","Default plugin location",std::string("plugins/"));
        plugins.add("location-2","Default plugin location for RobWork",buildDir+"/libs/"+RW_BUILD_TYPE+"/");
        plugins.add("location-3","Default plugin location for RobWorkStudio",buildDir+"/../RobWorkStudio/libs/"+RW_BUILD_TYPE+"/");
        plugins.add("location-4","Default plugin location for RobWorkSim",buildDir+"/../RobWorkSim/libs/"+RW_BUILD_TYPE+"/");
        plugins.add("location-5","Default plugin location for RobWorkHardware",buildDir+"/../RobWorkHardware/libs/"+RW_BUILD_TYPE+"/");

        _settings.add("plugins","List of plugins or plugin locations",plugins);
    }
    _settingsFile = rwsettingsPath;

    // get all plugin directories and files
    std::vector<std::string> cfgDirs;
    PropertyMap pluginsMap = _settings.get<PropertyMap>("plugins",PropertyMap());

    // add user defined plugin hints
    Log::infoLog() << "Adding plugins from arguments:\n";
    for(size_t i=0;i<plugins.size();i++){
    	std::stringstream sstr;
    	sstr << "loc-from-arg-" << i;
    	pluginsMap.add(sstr.str(), "Plugin location from init arguments", plugins[i]);
    }

    Log::infoLog() << "Looking for RobWork plugins in following directories:\n";
    BOOST_FOREACH( PropertyBase::Ptr prop , pluginsMap.getProperties()){
    	// check if its a
    	Property<std::string>::Ptr propstr = prop.cast<Property<std::string> >();
    	if(propstr==NULL)
    		continue;

    	cfgDirs.push_back( propstr->getValue() );
    	Log::infoLog() << "\t" << propstr->getValue() << std::endl;
    }

    Log::infoLog() << "Loading plugins:\n";
    BOOST_FOREACH(std::string dir, cfgDirs){
    	path file( dir );
    	Log::debugLog() << " processing hint: " << dir << std::endl;
#if(BOOST_FILESYSTEM_VERSION==2)
    	if( !file.has_root_path() ){
    		file = path( ipath.string() + "/" + dir );
    	}
#else
    	if( file.is_relative() ){
    		file = path( ipath.string() + "/" + dir );
    	}
#endif
    	Log::debugLog() << file.string() << std::endl;
    	if( !exists(file) )
    		continue;

        // now initialize plugin repository

        ExtensionRegistry::Ptr reg = ExtensionRegistry::getInstance();


    	// first check if its a directory or a file
    	if( is_directory(file) ){
    		// find all files in the directory *.rwplugin.xml *.rwplugin.(dll,so)
    		std::vector<std::string> pl_files =
    				IOUtil::getFilesInFolder(file.string(), false, true, "*.rwplugin.*");
    		BOOST_FOREACH(std::string pl_file, pl_files){
    		    Log::infoLog() << "\t "<< pl_file<< std::endl;
                rw::common::Ptr<Plugin> plugin = Plugin::load( pl_file );
                reg->registerExtensions(plugin);
    		}
    	} else {
    	    Log::debugLog() << "\t " <<  file.string() << std::endl;
            rw::common::Ptr<Plugin> plugin = Plugin::load( file.string() );
            reg->registerExtensions(plugin);
    	}
    }
}

namespace {
    rw::common::Ptr<rw::common::ExtensionRegistry> _extensionReg;
    rw::common::Log::Ptr _log;
    RobWork::Ptr _rwinstance;

    // this is used for initializing variables on program startup
    // hence, any program linked with this code will execute the constructor
    // before main(argc,argv) is entered...
    struct AutoInitializeRobWork {
       	AutoInitializeRobWork(){
       		//rw::common::Log::debugLog() << " AUTO INITILIZING ROBWORK .... " << std::endl;
        	if(_log==NULL)
        		_log =  rw::common::ownedPtr( new rw::common::Log() );
    		if(_extensionReg==NULL)
    			_extensionReg = rw::common::ownedPtr(new rw::common::ExtensionRegistry());
       		RobWork::getInstance()->initialize();

       	}
    } _initializer;
}

rw::common::Ptr<rw::common::ExtensionRegistry> RobWork::getExtensionRegistry(){
	// test for NULL, to avoid problems with 'static initialization order fiasco'
	if(_extensionReg==NULL)
		_extensionReg =  rw::common::ownedPtr(new rw::common::ExtensionRegistry());
    return _extensionReg;
}

void RobWork::setExtensionRegistry(rw::common::Ptr<rw::common::ExtensionRegistry> extreg){
    _extensionReg = extreg;
}


void RobWork::init(){
	RobWork::getInstance()->initialize();
}

void RobWork::init(int argc, const char** argv){
	// get log level, plugins, or plugin directories

    options_description desc("RobWork options");
    desc.add_options()
        ("help", "produce help message")
        ("rwloglevel", value<std::string>()->default_value("info"), "Set to debug, info, error, fatal")
        ("rwplugin", value<std::vector<std::string> >()->multitoken(), "Specific RobWork plugins or plugin directories to load. ");
    	("rwroot", value<std::string>(), "Directory of RobWork installation or development environment.");
    ;
    positional_options_description optionDesc;
    variables_map vm;
    store(command_line_parser(argc, argv).
              options(desc).positional(optionDesc).run(), vm);
    notify(vm);

    if (vm.count("help")) {
    	std::cout << "Usage example setting RobWork parameters:\n\n"
                  << "\t" << argv[0] <<" --rwloglevel=debug --rwplugin=/home/user/userplugin/libs/release/userplugin.rwplugin.so \n"
                  << "\t Plugins look like *.rwplugin.(dll,so,xml) \n"
                  << "\n";
    	std::cout << desc << "\n";
        return;
    }

    std::vector<std::string> plugins;
    if( vm.count("rwplugin") ){
    	plugins = vm["rwplugin"].as<std::vector<std::string> >();
    }


	RobWork::getInstance()->initialize(plugins);

	std::string rwloglevel_arg = vm["rwloglevel"].as<std::string>();
	if(rwloglevel_arg=="debug"){ Log::getInstance()->setLevel( Log::Debug ); }
	else if(rwloglevel_arg=="info"){ Log::getInstance()->setLevel( Log::Info ); }
	else if(rwloglevel_arg=="error"){ Log::getInstance()->setLevel( Log::Error ); }
	else if(rwloglevel_arg=="fatal"){ Log::getInstance()->setLevel( Log::Fatal ); }
	else { RW_WARN("rwloglevel set to unknown value!"); }
}

void RobWork::setLog(rw::common::Log::Ptr log){
	_log = log;
}

rw::common::Log& RobWork::getLog()
{
	// test for NULL, to avoid problems with 'static initialization order fiasco'
	if(_log==NULL)
		_log =  rw::common::ownedPtr( new rw::common::Log() );
    return *_log;
}

rw::common::Log::Ptr RobWork::getLogPtr()
{
	if(_log==NULL)
		_log =  rw::common::ownedPtr( new rw::common::Log() );
	return _log;
}

RobWork::Ptr RobWork::getInstance(){
	// test for NULL, to avoid problems with 'static initialization order fiasco'
	if(_rwinstance==NULL){
		_rwinstance = ownedPtr( new RobWork() );
	}
	return _rwinstance;
}

void RobWork::setInstance(RobWork::Ptr rw){
    _rwinstance = rw;
}
