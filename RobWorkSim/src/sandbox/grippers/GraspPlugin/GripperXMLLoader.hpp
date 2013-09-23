/**
 * @file GripperXMLLoader.hpp
 * @author Adam Wolniakowski
 */

#pragma once

#include <string>
#include "Gripper.hpp"



namespace rw {
	namespace loaders {
/**
 * @class GripperXMLLoader
 * @brief Class for loading grippers from xml.
 */
class GripperXMLLoader
{
	public:
	// static
		/**
		 * @brief Load gripper design from XML file
		 */
		static rw::models::Gripper::Ptr load(const std::string& filename);
		
		/**
		 * @brief Save gripper data to XML file
		 */
		static void save(rw::models::Gripper::Ptr gripper, const std::string& dir, const std::string& filename);
		
		
		/*
		 * @brief Save gripper results
		 */
		//static void saveResults(rw::models::Gripper::Ptr gripper, const std::string& filename);
	
	private:
};

}} // end namespaces
