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

#ifndef rw_math_EAA_HPP
#define rw_math_EAA_HPP

/**
 * @file EAA.hpp
 */

#include "Rotation3DVector.hpp"
#include "Rotation3D.hpp"
#include "Vector3D.hpp"

namespace rw { namespace math {

    /** @addtogroup math */
    /*@{*/

    /**
     * @brief A class for representing an equivalent angle-axis rotation
     *
     * This class defines an equivalent-axis-angle orientation vector also known
     * as an @f$ \thetak @f$ vector or "axis+angle" vector
     *
     * The equivalent-axis-angle vector is the product of a unit vector @f$
     * \hat{\mathbf{k}} @f$ and an angle of rotation around that axis @f$ \theta
     * @f$
     *
     * @note given two EAA vectors @f$ \theta_1\mathbf{\hat{k}}_1 @f$ and @f$
     * \theta_2\mathbf{\hat{k}}_2 @f$ it is generally not possible to subtract
     * or add these vectors, except for the special case when @f$
     * \mathbf{\hat{k}}_1 == \mathbf{\hat{k}}_2 @f$ this is why this class does
     * not have any subtraction or addition operators
     */
    template<class T = double>
    class EAA : public Rotation3DVector<T>
    {
    public:
        /**
         * @brief Extracts Equivalent axis-angle vector from Rotation matrix
         *
         * @param R [in] A 3x3 rotation matrix @f$ \mathbf{R} @f$
         *
         * @f$
         * \theta = arccos(\frac{1}{2}(Trace(\mathbf{R})-1)=arccos(\frac{r_{11}+r_{22}+r_{33}-1}{2})
         * @f$
         *
         * @f$
         * \thetak=log(\mathbf{R})=\frac{\theta}{2 sin \theta}(\mathbf{R}-\mathbf{R}^T) =
         * \frac{\theta}{2 sin \theta}
         * \left[
         * \begin{array}{c}
         * r_{32}-r_{23}\\
         * r_{13}-r_{31}\\
         * r_{21}-r_{12}
         * \end{array}
         * \right]
         * @f$
         *
         * @f$
         * \thetak=
         * \left[
         * \begin{array}{c}
         * 0\\
         * 0\\
         * 0
         * \end{array}
         * \right]
         * @f$ if @f$ \theta = 0 @f$
         *
         * @f$
         * \thetak=\pi
         * \left[
         * \begin{array}{c}
         * \sqrt{(R(0,0)+1.0)/2.0}\\
         * \sqrt{(R(1,1)+1.0)/2.0}\\
         * \sqrt{(R(2,2)+1.0)/2.0}
         * \end{array}
         * \right]
         * @f$ if @f$ \theta = \pi @f$
         *
         *
         * @todo beskriv special cases
         */
        EAA(const class Rotation3D<T>& R);

        /**
         * @brief Constructs an initialized EAA vector
         * @param axis [in] \f$ \mathbf{\hat{k}} \f$
         * @param angle [in] \f$ \theta \f$
         * @pre norm_2(axis) = 1
         */
        EAA(const Vector3D<T>& axis, T angle) :
            _eaa(axis * angle)
        {}

        /**
         * @brief Constructs an initialized EAA vector
         * @f$ \thetak =
         * \left[\begin{array}{c}
         *    \theta k_x\\
         *    \theta k_y\\
         *    \theta k_z
         * \end{array}\right]
         * @f$
         * @param thetakx [in] @f$ \theta k_x @f$
         * @param thetaky [in] @f$ \theta k_y @f$
         * @param thetakz [in] @f$ \theta k_z @f$
         */
        EAA(T thetakx, T thetaky, T thetakz) :
            _eaa(Vector3D<T>(thetakx, thetaky, thetakz)){}

        /**
         * @brief Constructs an initialized EAA vector
         *
         * The angle of the EAA are \f$\|eaa\|\f$ and the axis is \f$\frac{eaa}{\|eaa\|}\f$
         * @param eaa [in] Values to initialize the EAA
         */
        EAA(Vector3D<T> eaa) : _eaa(eaa) {}

        /**
         * @copydoc Rotation3DVector::toRotation3D()
         *
         * @f$
         * \mathbf{R} = e^{[\mathbf{\hat{k}}],\theta}=\mathbf{I}^{3x3}+[\mathbf{\hat{k}}] sin\theta+[{\mathbf{\hat{k}}}]^2(1-cos\theta) =
         *  \left[
         *    \begin{array}{ccc}
         *      k_xk_xv\theta + c\theta & k_xk_yv\theta - k_zs\theta & k_xk_zv\theta + k_ys\theta \\
         *      k_xk_yv\theta + k_zs\theta & k_yk_yv\theta + c\theta & k_yk_zv\theta - k_xs\theta\\
         *      k_xk_zv\theta - k_ys\theta & k_yk_zv\theta + k_xs\theta & k_zk_zv\theta + c\theta
         *    \end{array}
         *  \right]
         * @f$
         *
         * where:
         * - @f$ c\theta = cos \theta @f$
         * - @f$ s\theta = sin \theta @f$
         * - @f$ v\theta = 1-cos \theta @f$
         */
        virtual Rotation3D<T> toRotation3D() const;

        /**
         * @brief Extracts the angle of rotation @f$ \theta @f$
         * @return @f$ \theta @f$
         */
        T angle() const
        {
            return norm_2(_eaa.m());
        }

        /**
         * @brief Extracts the axis of rotation vector @f$ \mathbf{\hat{\mathbf{k}}} @f$
         * @return @f$ \mathbf{\hat{\mathbf{k}}} @f$
         */
        Vector3D<T> axis() const
        {
            T theta = angle();
            if (theta < 1e-6)
                return Vector3D<T>(0, 0, 0);
            else
                return _eaa / theta;
        }

        /**
         * @brief Returns element of EAA
         * @param i [in] index (@f$ 0 < i < 3 @f$)
         * @return the @f$ i @f$'th element
         */
        const T operator[](size_t i) const{
            assert(i < 3);
            return _eaa[i];
        }

        /**
         * @brief Returns element of EAA
         * @param i [in] index (@f$ 0 < i < 3 @f$)
         * @return the @f$ i @f$'th element
         */
        const T operator()(size_t i) const{
            assert(i < 3);
            return _eaa[i];
        }

        /**
         * @brief Calculates \f$ \robabx{a}{c}{\thetak} =
         * \robabx{a}{b}{\mathbf{R}} \robabx{b}{c}{\mathbf{\thetak}} \f$
         *
         * @param aRb [in] \f$ \robabx{a}{b}{\mathbf{R}} \f$
         * @param bTKc [in] \f$ \robabx{b}{c}{\thetak} \f$
         * @return \f$ \robabx{a}{c}{\thetak} \f$
         */
        friend EAA operator*(const Rotation3D<T>& aRb, const EAA& bTKc)
        {
            return EAA(aRb * bTKc._eaa);
            /* return Vector3D<T>(prod(aRb.m(), bTKc._eaa.m()))); */
        }

        /**
         * @brief Ouputs EAA to stream
         * @param os [in/out] stream to use
         * @param eaa [in] equivalent axis-angle
         * @return the resulting stream
         */
        friend std::ostream& operator<<(std::ostream& os, const EAA<T>& eaa){
            return os << eaa._eaa;
        }

        /**
         * @brief Calculates the cross product
         * @param v [in] a 3D vector
         * @param eaa [in] a 3D eaa vector
         * @return the resulting 3D vector
         */
        friend Vector3D<T> cross(const Vector3D<T>& v, const EAA<T>& eaa)
        {
            return cross(v, eaa._eaa);
        }

        /**
         * @brief Casts EAA<T> to EAA<Q>
         * @param eaa [in] EAA with type T
         * @return EAA with type Q
         */
        template<class Q>
        friend EAA<Q> cast(const EAA<T>& eaa) {
            return EAA<Q>(
                static_cast<Q>(eaa(0)),
                static_cast<Q>(eaa(1)),
                static_cast<Q>(eaa(2)));
        }

    private:
        Vector3D<T> _eaa;
    };

    /*@}*/

}} // end namespaces

#endif // end include guard
