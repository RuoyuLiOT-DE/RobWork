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

#ifndef RWHW_FANUC_VELRAMPPROFILE_HPP
#define RWHW_FANUC_VELRAMPPROFILE_HPP

/**
 * @file VelRampProfile.hpp
 */

#include <vector>

#include <rw/math/Q.hpp>

namespace rwhw { namespace fanuc {

    /** @addtogroup rwhw */
    /*@{*/


    /**
     * @brief velocity ramp profile
     */
    class VelRampProfile
    {
    public:
        /** @brief Over and upper limit.
         */
        typedef std::pair<double, double> Range;

        /**
         * @brief constructor
         * @param poslimits [in] The lower and upper position limits
         * @param vellimits [in] The lower and upper velocity limits
         * @param acclimits [in] The lower and upper accelleration limits
         */
        VelRampProfile(
            const std::vector<Range>& poslimits,
            const std::vector<Range>& vellimits,
            const std::vector<Range>& acclimits);

        /**
         * @brief deconstructor
         */
        ~VelRampProfile();

        /**
         * @brief Returns the velocity needed for moving towards goal, when
         * having the current position \f$pos\f$ and velocity \f$vel\f$.
         *
         * @param goal [in] the goal
         * @param pos [in] the current position
         * @param vel [in] the current velocity
         * @param dt [in] time interval
         */
        rw::math::Q getVelocity(
            const rw::math::Q& goal,
            const rw::math::Q& pos,
            const rw::math::Q& vel,
            double dt) const;

    private:
        std::vector<Range> _poslimits;
        std::vector<Range> _vellimits;
        std::vector<Range> _acclimits;
    };

    /**@}*/
}} // end namespaces

#endif // end include guard
