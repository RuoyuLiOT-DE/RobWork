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


#include "ClearanceOptimizer.hpp"
#include "ClearanceCalculator.hpp"

#include <rw/common/Log.hpp>
#include <rw/common/Timer.hpp>
#include <rw/math/Random.hpp>
#include <rw/models/Device.hpp>
#include <rw/pathplanning/StateConstraint.hpp>
#include <rw/pathplanning/QConstraint.hpp>

using namespace rw::math;
using namespace rw::common;
using namespace rw::kinematics;
using namespace rw::models;
using namespace rw::pathplanning;
using namespace rw::trajectory;
using namespace rwlibs::pathoptimization;

const std::string ClearanceOptimizer::PROP_STEPSIZE = "StepSize";
const std::string ClearanceOptimizer::PROP_LOOPCOUNT = "LoopCount";
const std::string ClearanceOptimizer::PROP_MAXTIME = "MaxTime";

ClearanceOptimizer::ClearanceOptimizer(const Device::CPtr& device,
                                       const State& state,
									   const QMetric::CPtr& metric,
                                       const ClearanceCalculator::CPtr& clearanceCalculator) :
	_device(device),
	_state(state),
	_metric(metric),
	_clearanceCalculator(clearanceCalculator),
	_stepsize(0.1)
{
	_dof = device->getDOF();
	_qConstraint = rw::pathplanning::QConstraint::makeBounds(device->getBounds());

    _propertymap.add<double>(PROP_STEPSIZE, "Step Size", 0.1);
    _propertymap.add<int>(PROP_LOOPCOUNT, "Maximal Number of Loops", 20);
    _propertymap.add<double>(PROP_MAXTIME, "Maximal Time to use (seconds)", 200.);

}

ClearanceOptimizer::~ClearanceOptimizer()
{
}

double ClearanceOptimizer::clearance(const Q& q) {
	_device->setQ(q, _state);
	const double d = _clearanceCalculator->clearance(_state);
	return d;
}

QPath ClearanceOptimizer::optimize(const QPath& inputPath) {
    return optimize(
        inputPath,
        _propertymap.get<double>(PROP_STEPSIZE),
        _propertymap.get<int>(PROP_LOOPCOUNT),
        _propertymap.get<double>(PROP_MAXTIME) );
}

QPath ClearanceOptimizer::optimize(const QPath& inputPath, double stepsize, size_t maxcount, double maxtime) {
	if(maxcount == 0 && maxtime <= 0) {
		RW_THROW("No stopping criteria was set.");
	}
    _stepsize = stepsize;
	if (inputPath.size() < 2)
		return inputPath;
	Timer timer;
	timer.reset();

	AugmentedPath path;

	subDivideAndAugmentPath(inputPath, path);
	if (path.size() <= 2)
		return inputPath;
	size_t cnt = 0;
	while ( (maxcount == 0 || cnt < maxcount) && (maxtime <= 0 || timer.getTime() < maxtime)) {
		AugmentedPath newPath = path;
		const Q dir = randomDirection();
        
        // verify that the path is not already fully optimized
        bool fully_optimized = true;
        for (AugmentedPath::const_iterator it = ++(newPath.cbegin()); it != --(newPath.cend()); ++it) {
            if(it->second < _minClearance){
                fully_optimized = false;
                break;
            }
        }
        if(fully_optimized){
            rw::common::Log::log().debug() << "Stoped clearance optimization early due to completion. Took " << cnt << " iterations and " << timer.getTime() << " seconds." << std::endl;
            break;
        }
        
#ifdef RW_HAVE_OMP
        // optimize the remaining path
#pragma omp parallel for schedule(dynamic,1)
		for (int q = 0; q < static_cast<int>(newPath.size()); ++q) {
			AugmentedPath::iterator it = newPath.begin() + q;
#else
		for (AugmentedPath::iterator it = ++(newPath.begin()); it != --(newPath.end()); ++it) {
#endif
			const Q qnew = it->first + dir;
			if (it->second < _minClearance && !_qConstraint->inCollision(qnew)) {
#ifdef RW_HAVE_OMP
                rw::kinematics::State tmp_state = _state;
#else
				rw::kinematics::State &tmp_state = _state;
#endif
				_device->setQ(qnew, tmp_state);
				if (_stateConstraint != nullptr && _stateConstraint->inCollision(tmp_state)) {
					continue;
				}
				if (_qConstraintUser != nullptr && _qConstraintUser->inCollision(qnew)) {
					continue;
				}
			    const double newClearance = _clearanceCalculator->clearance(tmp_state);
			    if (it->second < newClearance) {
					it->first = qnew;
					it->second = newClearance;
			    }
			}
		}

		path = validatePath(newPath, path);
		removeBranches(path);
		++cnt;
	}

	QPath result;
    result.reserve(path.size());
	for (AugmentedPath::const_iterator it = path.cbegin(); it != path.cend(); ++it) {
	    result.push_back((*it).first);
	}
	return result;
}

//Implements the ValidatePath from [1]
ClearanceOptimizer::AugmentedPath ClearanceOptimizer::validatePath(const AugmentedPath& newPath, const AugmentedPath& orgPath) {
    AugmentedPath result;
#ifdef RW_HAVE_OMP
    result.reserve(newPath.size());
#endif
    AugmentedPath::const_iterator it_new = newPath.begin();
    AugmentedPath::const_iterator it_next = ++newPath.begin();
    AugmentedPath::const_iterator it_org = ++orgPath.begin();

    while (it_next != newPath.end()) {
        //std::cout<<"A"<<(*it_new).first<<" "<<(*it_next).first<<std::endl;
        result.push_back(*it_new);
        if (_metric->distance((*it_new).first, (*it_next).first) > _stepsize) {
            Q qint = interpolate((*it_new).first, (*it_next).first, 0.5);
            const double clearanceInt = clearance(qint);
            if (clearanceInt > (*it_org).second) {
                result.push_back(AugmentedQ(qint, clearanceInt));
            } else {
                result.push_back(*it_org);
            }
        }
        ++it_new;
        ++it_next;
        ++it_org;
    }
    result.push_back(*it_new);

    return result;
}

//Implements the RemoveBranches algorithm from [1]
void ClearanceOptimizer::removeBranches(AugmentedPath& path) const {
   AugmentedPath::iterator it1 = path.begin();
   AugmentedPath::iterator it2 = path.begin();
   ++it2;
   AugmentedPath::iterator it3 = path.begin();
   ++it3;
   ++it3;
   while (it3 != path.end()) {
       if (_metric->distance((*it1).first, (*it3).first) < _stepsize) {
           it3 = path.erase(it2);
           it2 = it3; --it2;
           if (it2 == path.begin()) {
               it1 = it2;
               ++it2;
               ++it3;
           } else {
               it1 = it2;
               --it1;
           }
           if (it1 != path.begin()) {
               --it1;
               --it2;
               --it3;
           }
       } else {
           ++it1;
           ++it2;
           ++it3;
       }
   }
}


double ClearanceOptimizer::calcAvgClearance(const AugmentedPath& path) const {
    double sum = 0;
    for (AugmentedPath::const_iterator it = path.begin(); it != path.end(); ++it) {
        sum += (*it).second;
    }
    sum /= static_cast<double>(path.size());
    return sum;
}

//Interpolates between to nodes. At some point we experiment with other ways of
//interpolating, which is why it is left as a seperate method.
Q ClearanceOptimizer::interpolate(const Q& q1, const Q& q2, double ratio) const {
    return q1*ratio+(1.0-ratio)*q2;
}


void ClearanceOptimizer::subDivideAndAugmentPath(const QPath& path, AugmentedPath& result) {
    Timer timer1;
#ifndef RW_HAVE_OMP
    Timer timer2;
	timer2.resetAndPause();
#endif
    timer1.reset();
	QPath::const_iterator itstart = path.begin();
	QPath::const_iterator itnext = path.begin();
	++itnext;
	for ( ; itnext != path.cend(); ++itstart, ++itnext) {
		Q delta = (*itnext) - (*itstart);
		const double fraction = _metric->distance(delta)/_stepsize;
		const int divisions = static_cast<int>(ceil(fraction));
		delta /= static_cast<double>(divisions);
#ifdef RW_HAVE_OMP
		AugmentedPath tmp_res(divisions);
#pragma omp parallel for schedule(dynamic,1)
#endif
		for (int i = 0; i < divisions; ++i) {
		    const Q q = (*itstart) + delta*static_cast<double>(i);
#ifdef RW_HAVE_OMP
			rw::kinematics::State tmp_state = _state;
			_device->setQ(q, tmp_state);
			const double dist = _clearanceCalculator->clearance(tmp_state);
			tmp_res[i] = AugmentedQ(q, dist);
#else
            timer2.resume();
			const double dist = clearance(q);
			timer2.pause();
			result.push_back(AugmentedQ(q, dist));
#endif
		}
#ifdef RW_HAVE_OMP
#ifndef __GNUC__
		result.insert(result.cend(), tmp_res.cbegin(), tmp_res.cend());
#elif __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 8)
		result.insert(result.cend(), tmp_res.cbegin(), tmp_res.cend());
#else
		result.insert(result.end(), tmp_res.cbegin(), tmp_res.cend());
#endif
#endif
	}
//	std::cout<<"SubDivide Finished "<<result.size()<<std::endl;
	result.push_back(AugmentedQ(path.back(), clearance(path.back())));
#ifdef RW_HAVE_OMP 
	rw::common::Log::log().debug() << "Time to subdivided and augment " << timer1.getTime() << std::endl;
#else
	rw::common::Log::log().debug() << "Time to subdivided and augment " << timer1.getTime() << "  " << timer2.getTime() << std::endl;
#endif
}

//Calculates a random direction.
//To get an equal distribution in all directions (at least for a 2-norm based metric) a
//do-while loop is used, which checks that the direction vector is less than stepsize,
//before scaling it. Otherwise we might get an uneven distribution.
//Assuming a relationship between the output of the metric and the selected stepsize, the method
//uses stepsize instead of 1 as bounds on the random numbers.
Q ClearanceOptimizer::randomDirection() const {
	Q q(_dof);
	do {
	    for (size_t i = 0; i < _dof; ++i)
	        q(i) = Random::ran(-_stepsize, _stepsize);
	} while (_metric->distance(q) > _stepsize);

	q *= (_stepsize / _metric->distance(q));
	return q;
}


PropertyMap& ClearanceOptimizer::getPropertyMap() {
    return _propertymap;
}

void ClearanceOptimizer::setMinimumClearance(const double dist) {
	if(dist < 0){
		RW_THROW("Minimum clearance must be greater or equal to zero.");
	}
	_minClearance = dist;
}
    
double ClearanceOptimizer::getMinimumClearance() const {
	return _minClearance;
}

void ClearanceOptimizer::setStateConstraint(const StateConstraint::CPtr& stateConstraint) {
	_stateConstraint = stateConstraint;
}

void ClearanceOptimizer::setQConstraint(const QConstraint::CPtr& qConstraint) {
	_qConstraintUser = qConstraint;
}

const ClearanceCalculator::CPtr& ClearanceOptimizer::getClearanceCalculator() const {
    return _clearanceCalculator;
}
