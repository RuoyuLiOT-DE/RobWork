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

#include "../TestSuiteConfig.hpp"
#include <rw/math/Transform3D.hpp>

#include <rw/math/Rotation3D.hpp>
#include <rw/math/Vector3D.hpp>
#include <rw/math/EAA.hpp>
#include <rw/math/Constants.hpp>


using namespace rw::math;

namespace
{
    double norm_inf(const Vector3D<>& v)
    {
        return norm_inf(v.m());
    }
}

BOOST_AUTO_TEST_CASE(Transform3DTest){
    BOOST_TEST_MESSAGE("- Testing Transform3D");
    const Vector3D<> d(1, 2, 3);
    const Transform3D<> t(d);

    const Vector3D<> v1(0, 0, 0);
    BOOST_CHECK(norm_inf(t * v1 - d) == 0);

    const EAA<> eaa(Pi / 2, 0, 0);
    const Rotation3D<> r1 = eaa.toRotation3D();
    const Transform3D<> t2(d, r1);
    const Transform3D<> t3 = t2 * inverse(t2);

    const EAA<> xeaa(t3.R());
    BOOST_CHECK(norm_inf(xeaa.axis()) == 0);
    BOOST_CHECK(xeaa.angle() == 0);
    BOOST_CHECK(norm_inf(t3.P()) < 1e-15);

    Transform3D<float> tf;
    tf = cast<float>(t);
    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 4; j++)
            BOOST_CHECK(tf(i, j) == (float)t(i, j));
    tf = rw::math::cast<float>(t); // qualified lookup
    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 4; j++)
            BOOST_CHECK(tf(i, j) == (float)t(i, j));

    /* Test comparison operators operator== and operator!= */
    const EAA<> eaacomp1(Pi / 2, 0, 0);
    const Rotation3D<> rotcomp1 = eaacomp1.toRotation3D();
    const Vector3D<double> comp1(1.1, -2.2, 3.3);
    const Transform3D<double> tcomp1(comp1, rotcomp1);

    const EAA<> eaacomp2(Pi / 2, 0, 0);
    const Rotation3D<> rotcomp2 = eaacomp2.toRotation3D();
    const Vector3D<double> comp2(1.1, -2.2, 3.3);
    const Transform3D<double> tcomp2(comp2, rotcomp2);

    BOOST_CHECK(tcomp1 == tcomp2);
    BOOST_CHECK(!(tcomp1 != tcomp2));

    const EAA<> eaacomp3(Pi / 4, 0, 0);
    const Rotation3D<> rotcomp3 = eaacomp3.toRotation3D();
    const Vector3D<double> comp3(1.1, -2.2, 3.3);
    const Transform3D<double> tcomp3(comp3, rotcomp3);
    BOOST_CHECK(tcomp1 != tcomp3);
    BOOST_CHECK(!(tcomp1 == tcomp3));
}
