/*********************************************************************
 * RobWork Version 0.2
 * Copyright (C) Robotics Group, Maersk Institute, University of Southern
 * Denmark.
 *
 * RobWork can be used, modified and redistributed freely.
 * RobWork is distributed WITHOUT ANY WARRANTY; including the implied
 * warranty of merchantability, fitness for a particular purpose and
 * guarantee of future releases, maintenance and bug fixes. The authors
 * has no responsibility of continuous development, maintenance, support
 * and insurance of backwards capability in the future.
 *
 * Notice that RobWork uses 3rd party software for which the RobWork
 * license does not apply. Consult the packages in the ext/ directory
 * for detailed information about these packages.
 *********************************************************************/

#ifndef rw_math_Math_HPP
#define rw_math_Math_HPP

/**
 * @file Math.hpp
 */

#include <cmath>
#include "LinearAlgebra.hpp"
#include "EAA.hpp"
#include "RPY.hpp"
#include "Quaternion.hpp"
#include "Q.hpp"

namespace rw { namespace math {

    /** @addtogroup math */
    /*@{*/

    /**
     * @brief A collection of different math rutines
     */
    class Math
    {
    public:
        /**
         * @brief this function converts a Quaternion to an EAA
         * @param quat [in] - the Quaternion object that is to be converted
         * @return a EAA object that represents the converted quaternion
         */
        template <class A>
        static EAA<A> QuaternionToEAA(const Quaternion<A> &quat)
        {
            Quaternion<A> q = quat;
            A angle,kx,ky,kz;

            // if w > 1 acos and sqrt will produce errors, this cant happen if
            // quaternion is normalised.
            if (q.getQw() > 1)
                q.normalize();

            angle = 2 * acos(q.getQw());

            // assuming quaternion normalised then w is less than 1, so term
            // always positive.
            double s = sqrt(1 - q.getQw() * q.getQw());

            // test to avoid divide by zero, s is always positive due to sqrt
            if (s < 0.001) {
                // if s close to zero then direction of axis not important

                // if it is important that axis is normalised then replace with
                // x=1; y=z=0;
                kx = q.getQx();
                ky = q.getQy();
                kz = q.getQz();
            } else {
                kx = q.getQx() / s; // normalise axis
                ky = q.getQy() / s;
                kz = q.getQz() / s;
            }
            return EAA<A>(kx*angle,ky*angle,kz*angle);
        };

        /**
         * @brief this function converts a EAA object to a Quaternion
         *
         * @param eaa [in] - the EAA object that is to be converted
         *
         * @return a Quaternion object that represents the converted EAA
         */
        template <class A>
        static Quaternion<A> EAAToQuaternion(const EAA<A> &eaa)
        {
            Vector3D<A> v = eaa.axis();
            A s = sin(eaa.angle()/2);
            A x = v[0] * s;
            A y = v[1] * s;
            A z = v[2] * s;
            A w = cos(eaa.angle()/2);
            return Quaternion<A>(x,y,z,w);
        };

        /**
         * @brief this function converts a EAA object to a Quaternion
         *
         * @param roll [in] rotation around z
         * @param pitch [in] rotation around y
         * @param yaw [in] rotation around x
         *
         * @return a Quaternion object that represents the converted EAA
         */
        template <class A>
        static Rotation3D<A> ZYXToRotation3D(A roll, A pitch, A yaw)
        {
            return RPY<A>(roll, pitch, yaw).toRotation3D();
        };

        /**
         * \brief Constructs a 3x3 skew-symmetric matrix \f$ S\in so(3)\f$
         * \param s [in] the \f$ s_x \f$, \f$ s_y \f$ and \f$ s_z \f$ of the matrix
         * \return The 3x3 skew-symmetric matrix \f$S\f$
         *
         * \f$
         * S =
         * \left[
         * \begin{array}{ccc}
         *    0 & -s_z &  s_y\\
         *  s_z &    0 & -s_x\\
         * -s_y &  s_x &    0
         * \end{array}
         * \right]
         * \f$
         */
        template<class R>
        static inline boost::numeric::ublas::bounded_matrix<R, 3, 3> Skew(
            const boost::numeric::ublas::bounded_vector<R, 3>& s)
        {
            boost::numeric::ublas::bounded_matrix<R, 3, 3> S;
            S(0,0) =   0.0; S(0,1) = -s[2]; S(0,2) =  s[1];
            S(1,0) =  s[2]; S(1,1) =   0.0; S(1,2) = -s[0];
            S(2,0) = -s[1]; S(2,1) =  s[0]; S(2,2) =   0.0;

            assert(LinearAlgebra::IsSkewSymmetric(S));
            return S;
        }

        /**
         * @brief clamp val to either min or max
         *
         * @param val [in] the value that is to be clamped
         * @param min [in] the minimum allowed value
         * @param max [in] the maximum allowed value
         * @return the clamped value of val
         */
        static double Clamp(double val, double min, double max)
        {
            if (val < min)
                return min;
            if(val > max)
                return max;
            return val;
        }

        /**
         * @brief Clamps values of \b q with \b min and \b max
         *
         * @param q [in] Values to clamp
         * @param min [min] The minimum value
         * @param max [min] The maximum value
         * @return The clamped values
         */
        static rw::math::Q ClampQ(
            const rw::math::Q& q,
            const rw::math::Q& min,
            const rw::math::Q& max)
        {
            assert(q.size() == min.size());
            assert(q.size() == max.size());

            Q qres(q.size());
            for (size_t i = 0; i<q.size(); i++)
                qres(i) = Clamp(q(i), min(i), max(i));

            return qres;
        }

        // Global random number generation.

        /*
         * @brief Reset the global random number generator.
         */
        // void reset(long seed);

        /**
         * @brief A random double in the range [0, 1[.
         *
         * @note Uses boost::random
         */
        static double Ran();

        /**
         * @brief Seeds the random number generator.
         *
         * @note Uses boost::random
         */
        static void Seed(unsigned seed);

        /**
         * @brief A random double in the range [from, to[.
         *
         * @note Uses boost::random
         */
        static double Ran(double from, double to);

        /**
         * @brief A random integer in the range [from, to[.
         *
         * @note Uses boost::random
         */
        static int RanI(int from, int to);

        /**
         * @brief Returns a random sample around \mean with standard deviation \b sigma
         *
         * @note Uses boost::random
         *
         * @param mean [in] Means value
         * @param sigma [in] Standard deviation
         * @return Random sample
         *
         */
        static double RanNormalDist(double mean, double sigma);

        /**
         * @brief Returns a random Q between with values in the range [from, to[.
         *
         * @note Uses boost::random
         *
         * @param from [in] The lower bound
         * @param to [in] The upper bound
         * @return Random Q
         */
        static rw::math::Q RanQ(const rw::math::Q& from, const rw::math::Q& to);

        /**
         * @brief Rounds off to nearest integer
         *
         * With some compilers \b round can be found in math.h. This however does not
         * appear to be ansi C/C++ standard
         *
         * @param d [in] number to round
         * @return d rounded to nearest integer.
         */
        static double Round(double d) {
            return floor(d + 0.5);

        }

        /**
         * @brief Squares \b d
         * @param d [in] Number to square
         * @return The square
         */
        static inline double Sqr(double d) {
            return d*d;
        }
        
        
        
        /**
          * @brief Returns vector with the absolute values
          *
          * Given a vector \f$v=[v_1,v_2,\ldots,v_n]\f$ then Abs(v) is defined as
          * \f$Abs(v)=[abs(v_1),abs(v_i),\ldots,abs(v_n)] \f$
          *
          * @param v [in] the vector \f$v\f$
          * @return the vector \f$Abs(v)\f$
          */
         static Q Abs(const Q& v)
         {
             Q result(v.size());
             for (size_t i = 0; i<v.size(); i++)
                 result[i] = std::fabs(v[i]);
             return result;
         }
 
         /** 
          * @brief Returns the smallest element of v
          *
          * If the vector has zero length, the method returns 0
          *
          * @param v [in] the vector v
          * @return the smallest element
          */
         static double Min(const Q& v)
         {
             if (v.size() == 0)
                 return 0;
             
             double minval = v(0);
             for (size_t i = 1; i<v.size(); i++)
                 if (v(i)<minval)
                     minval = v(i);
             return minval;
         }

         /**
          * @brief Returns the largest element of v
          *
          * If the vector has zero length, the method returns 0
          *
          * @param v [in] the vector v
          * @return the largest element
          */
         static double Max(const Q& v)
         {
             if (v.size() == 0)
                 return 0;
             double maxval = v(0);
             for (size_t i = 1; i<v.size(); i++)
                 if (v(i)>maxval)
                     maxval = v(i);
             return maxval;
         }

         
         /**
          * @brief Returns vector with the elementwise smallest elements of \b a and \b b
          *
          * @param a [in] the vector \b a
          * @param b [in] the vector \b b
          * @return Q with smallest elements
          */
         template<class T>
         static T Min(const Q& a, const Q& b)
         {
             RW_ASSERT(a.size() == b.size());
             
             Q result(a.size());
             for (size_t i = 1; i<a.size(); i++)
                 result(i) = std::min(a(i), b(i));
             return result;
         }

         /**
          * @brief Returns vector with the elementwise largest elements of \b a and \b b
          *
          * @param v [in] the vector \b a
          * @param b [in] the vector \b b          
          * @return Q with largest elements
          */
         template<class T>
         static T Max(const Q& a, const Q& b)
         {
             RW_ASSERT(a.size() == b.size());
             
             Q result(a.size());
             for (size_t i = 1; i<a.size(); i++)
                 result(i) = std::max(a(i), b(i));
             return result;
         }
         
         /**
           * @brief Returns vector with the absolute values
           *
           * Given a vector \f$v=[v_1,v_2,\ldots,v_n]\f$ then Abs(v) is defined as
           * \f$Abs(v)=[abs(v_1),abs(v_i),\ldots,abs(v_n)] \f$
           *
           * @param v [in] the vector \f$v\f$
           * @return the vector \f$Abs(v)\f$
           */
         template<class T>
         static Vector3D<T> Abs(const Vector3D<T>& v)
         {
             Vector3D<T> result;
             for (size_t i = 0; i<3; i++)
                 result[i] = std::fabs(v[i]);
             return result;
         }

         /**
          * @brief Returns the smallest element of v
          *
          * @param v [in] the vector v
          * @return the smallest element
          */
         template<class T>
         static T Min(const Vector3D<T>& v)
         {
             T minval = v(0);
             for (size_t i = 1; i<3; i++)
                 if (v(i)<minval)
                     minval = v(i);
             return minval;
         }

         /**
          * @brief Returns the largest element of v
          *
          * @param v [in] the vector v
          * @return the largest element
          */
         template<class T>
         static T Max(const Vector3D<T>& v)
         {
             T maxval = v(0);
             for (size_t i = 1; i<3; i++)
                 if (v(i)>maxval)
                     maxval = v(i);
             return maxval;
         }
         
         
         
         /**
          * @brief Returns vector with the elementwise smallest elements of \b a and \b b
          *
          * @param a [in] the vector \b a
          * @param b [in] the vector \b b
          * @return Vector with smallest elements
          */
         template<class T>
         static Vector3D<T> Min(const Vector3D<T>& a, const Vector3D<T>& b)
         {
             Vector3D<T> result;
             for (size_t i = 1; i<3; i++)
                 result(i) = std::min(a(i), b(i));
             return result;
         }

         /**
          * @brief Returns vector with the elementwise largest elements of \b a and \b b
          *
          * @param v [in] the vector \b a
          * @param b [in] the vector \b b          
          * @return Vector with largest elements
          */
         template<class T>
         static Vector3D<T> Max(const Vector3D<T>& a, const Vector3D<T>& b)
         {
             Vector3D<T> result;
             for (size_t i = 1; i<3; i++)
                 result(i) = std::max(a(i), b(i));
             return result;
         }
        



         
         
    };

    /*@}*/
}} // end namespaces

#endif // end include guard
