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

#include "GraspPolicyFactory.hpp"

using namespace rwsim::util;
using namespace rwsim;

#include "TargetConfigGraspPolicy.hpp"

std::vector<std::string> GraspPolicyFactory::getAvailablePolicies()
{
	std::vector<std::string> policies;
	policies.push_back( TargetConfigGraspPolicy::getID() );
	policies.push_back("TargetVelocity");
	return policies;
}

GraspPolicy::Ptr GraspPolicyFactory::makePolicy(
		const std::string& id,
		dynamics::DynamicWorkCell* dwc,
		rwsim::dynamics::DynamicDevice* dev)
{
	if( id ==  TargetConfigGraspPolicy::getID() ){
		return rw::common::ownedPtr( new TargetConfigGraspPolicy( dev ) );
	} else if(id == "TargetVelocity" ){

	}

	RW_THROW("No such policy! ");
	return NULL;
}
