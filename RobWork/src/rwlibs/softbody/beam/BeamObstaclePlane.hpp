/*
    Copyright 2013 <copyright holder> <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#ifndef BEAMOBSTACLEPLANE_HPP
#define BEAMOBSTACLEPLANE_HPP

#include <rw/geometry/Plane.hpp>
#include <rw/math/Transform3D.hpp>



namespace rwlibs {
namespace softbody {
    class BeamObstaclePlane {
    public:
        BeamObstaclePlane(const rw::geometry::Plane &plane, const rw::math::Transform3D<> &trans);
        
        rw::math::Transform3D<> getTransform(void) const;
        void setTransform(const rw::math::Transform3D<> &trans);
        
        double get_yTCP(const rw::math::Transform3D<> &planeTbeam) const;
        double get_thetaTCP(const rw::math::Transform3D<> &planeTbeam) const;
        
        rw::math::Transform3D<> compute_planeTbeam(const rw::math::Transform3D<> &Tbeam);
        
        const rw::geometry::Plane &getPlane(void) const;
        
        
    private:
        rw::geometry::Plane _plane;
        rw::math::Transform3D<> _trans;
        
        
    };
}}

#endif // BEAMOBSTACLEPLANE_HPP