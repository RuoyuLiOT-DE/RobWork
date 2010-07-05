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

#ifndef RWLIBS_DRAWABLE_RENDERSCAN_HPP_
#define RWLIBS_DRAWABLE_RENDERSCAN_HPP_

//! @file RenderScan.hpp

#include <rwlibs/os/rwgl.hpp>
#include <rw/sensor/Image25D.hpp>
#include <rw/sensor/Scan2D.hpp>
#include "RWGLTexture.hpp"
#include "Render.hpp"

namespace rwlibs {
namespace drawable {

	//! @addtogroup drawable
	// @{


    /**
     * @brief renders Image25D, Scan2D or a simple distance.
     */
	class RenderScan: public Render{
	public:
		RenderScan();
		virtual ~RenderScan();

		/**
		 * \brief set a 2.5 dimensional scan
		 * @param img
		 */
		void setScan(const rw::sensor::Image25D& img);

		/**
		 * \brief set a two dimensional scan
		 * @param img
		 */
		void setScan(const rw::sensor::Scan2D& img);

		/**
		 * \brief set a one dimensional scan
		 * @param dist
		 */
		void setScan(float dist);

        /**
         * @brief the minimum depth of the points, where depth is in
         * the negative z-axis direction
         * @param depth [in] depth in the negative z-axis direction
         */
        void setMinDepth(float depth){ _minDepth=depth;};

        /**
         * @brief the maximum depth of the points, where depth is in
         * the negative z-axis direction
         * @param depth [in] depth in the negative z-axis direction
         */
        void setMaxDepth(float depth){ _maxDepth=depth;};

        /**
         * @copydoc Render::draw
         */
        void draw(DrawType type, double alpha) const;

	private:
        rw::sensor::Image25D _img;
        float _minDepth,_maxDepth;
	};

	typedef rw::common::Ptr<RenderScan> RenderScanPtr;

	//! @}
}
}
#endif /* RENDERSCAN_HPP_ */
