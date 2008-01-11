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

#ifndef rw_kinematics_FramePropertyImpl_HPP
#define rw_kinematics_FramePropertyImpl_HPP

/**
 * @file FramePropertyImpl.hpp
 */

#include "FrameProperty.hpp"
#include "Frame.hpp"
#include <rw/common/macros.hpp>
#include <rw/common/StringUtil.hpp>
#include <boost/shared_ptr.hpp>

namespace rw { namespace kinematics {

    /**
     * @brief An implementation of the FrameProperty class.
     */
    template <typename T>
    class FramePropertyImpl : public FrameProperty<T>
    {
    public:
        /**
         * @brief An accessor for a named property of a frame.
         *
         * @param key [in] The name of property to access.
         *
         * @param description [in] The description of the property.
         */
        FramePropertyImpl(const std::string& key, const std::string& description) :
            _key(key),
            _description(description)
        {}

        /**
         * @brief The name of the property that is being accessed.
         *
         * @return The name of the property.
         */
        const std::string& key() const { return _key; }

        /**
         * @brief The value of the property of a frame.
         *
         * @param frame [in] A frame containing properties.
         *
         * @return A pointer to the value of the property of the frame or NULL
         * if the property does not exist or if the value of the property is of
         * a wrong type.
         */
        T* getPtr(Frame& frame) const
        {
            common::Property<T>* prop;
            common::PropertyMap& map = frame.getPropertyMap();
            prop = map.getProperty<T>(_key);
            if (prop) {
                return &prop->getValue();
            } else
                return NULL;
        }

        /**
         * @copydoc getPtr
         */
        const T* getPtr(const Frame& frame) const
        {
            // Forward to non-const version.
            return getPtr(const_cast<Frame&>(frame));
        }

        /**
         * @brief True iff the frame has the property.
         *
         * A call of the method is equivalent to
         \code
         getPtr(frame) != 0
         \endcode
         *
         * @param frame [in] A frame containing properties.
         *
         * @return \a true iff the property exists in \a frame.
         */
        bool has(const Frame& frame) const
        {
            return this->getPtr(frame) != NULL;
        }

        /**
         * @brief The value of the property of a frame.
         *
         * The method is superflous as it can be easily implemented in terms of
         * getPtr().
         *
         * @param frame [in] A frame containing properties.
         *
         * @return The value of the property of the frame.
         *
         * The property must be present in the frame or the program will throw.
         * If you don't like that policy you should use getPtr().
         */
        const T& get(const Frame& frame) const
        {
            const T* value = this->getPtr(frame);
            if (!value) {
                // Perhaps this error message should include the description of
                // the property also.
                RW_THROW(
                    "No key (of some type T) named "
                    << rw::common::StringUtil::Quote(key())
                    << " in frame "
                    << rw::common::StringUtil::Quote(frame.getName()));

            }

            return *value;
        }

        /**
         * @brief Assign a value to the property of the frame.
         *
         * @param frame [in] A frame containing properties.
         *
         * @param value [in] The value to assign to the property.
         *
         * [NB: We haven't quite decided yet what to do if a value is already
         * present (we could have single-assignment) or if the value present is
         * of a wrong type.]
         */
        void set(Frame& frame, const T& value) const
        {
            T* propertyValue = this->getPtr(frame);
            if (propertyValue) {
                // We change the value, but forget to change the description.
                *propertyValue = value;
            } else {

                common::PropertyMap& map = frame.getPropertyMap();
                const bool ok = map.addProperty(_key, _description, value);

                if (!ok)
                    RW_THROW(
                        "Value of key "
                        << rw::common::StringUtil::Quote(key())
                        << " is already present in frame "
                        << rw::common::StringUtil::Quote(frame.getName())
                        << ", but the type of the value "
                        "differs from the expected.");
            }
        }

        void erase(Frame& frame) const
        {
            frame.getPropertyMap().removeProperty(_key);
        }

    private:
        std::string _key;
        std::string _description;
    };

    /*@}*/
}} // end namespaces

#endif // end include guard
