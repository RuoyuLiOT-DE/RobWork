/********************************************************************************
 * Copyright 2013 The Robotics Group, The Maersk Mc-Kinney Moller Institute,
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

#ifndef RWLIBS_OPENGL_RENDERFORCETORQUE_HPP
#define RWLIBS_OPENGL_RENDERFORCETORQUE_HPP

/**
 * @file RenderFrame.hpp
 */

#include <rwlibs/os/rwgl.hpp>

#include <rw/graphics/Render.hpp>
#include <rw/math/EAA.hpp>

namespace rwlibs { namespace opengl {

    /** @addtogroup drawable */
    /*@{*/

    /**
     * @brief RenderFrame makes a visualization of a frame
     */
    class RenderForceTorque : public rw::graphics::Render
    {
    public:
        //! @brief smart pointer type to this class
        typedef rw::common::Ptr<RenderForceTorque> Ptr;

        /**
         * @brief Constructs a RenderFrame
         * @param size [in] size of the frame coordinate system
         */
        RenderForceTorque();

        /**
         * @brief Destructor
         */
        virtual ~RenderForceTorque();

        void setForce(rw::math::Vector3D<> force);
        void setTorque(rw::math::Vector3D<> torque);

    	/* Functions inherited from Render */

        //! @copydoc rw::graphics::Render::draw(const DrawableNode::RenderInfo& info, DrawableNode::DrawType type, double alpha) const
        void draw(const rw::graphics::DrawableNode::RenderInfo& info,
                  rw::graphics::DrawableNode::DrawType type,
                  double alpha) const;

    private:
        void drawTorus(float R, float r, int N, int n);

        rw::math::Vector3D<> _force, _torque;
        mutable GLUquadricObj *_quadratic;
    };

    /*@}*/
}} // end namespaces

#endif // end include guard