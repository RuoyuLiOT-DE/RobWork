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

#ifndef RWHW_IEICANPORT_HPP
#define RWHW_IEICANPORT_HPP


#include "../CanPort.hpp"
#include <rw/common/macros.hpp>
#include <iostream>

namespace rwhw {

    /** @addtogroup can */
    /*@{*/

    /**
     * @brief CanPort driver wrapper for the IEICAN02 driver.
     */
    class IEICANPort: public CanPort
    {
    private:
        /**
         * @brief Constructs IEICANPort for card with index \b cardIdx and with port number \b portNr
         */
        IEICANPort(unsigned int cardIdx,unsigned int portNr);

        /**
         * @brief Destructor
         */
        virtual ~IEICANPort();

    public:
        /**
         * Gets an instance of IEICANPort by specifiing card and port nr.
         */
        static IEICANPort* getIEICANPortInstance(
            unsigned int cardIdx,
            unsigned int portNr); // TODO: add baud ad can id type

        /**
         * @copydoc CanPort::isOpen
         */
        bool isOpen();

        /**
         * @copydoc CanPort::open
         */
        bool open(/* baudrate, 11/29bit option,  */);

        /**
         * @copydoc CanPort::open
         */
		bool open(int idlow, int idhigh)
		{
			RW_THROW("Method NOT IMPLEMENTED !!!");
			return false;
		}

        /**
         * @copydoc CanPort::close
         */
        void close();

        /**
         * @copydoc CanPort::read
         */
        bool read( CanPort::CanMessage  &msg);

        /**
         * @copydoc CanPort::write
         */
        bool write(unsigned int id, const std::vector<unsigned char>& data);

    private:
        unsigned int _cardIdx;
        unsigned int _portNr;
        bool _portOpen;
    };

    /*@}*/

}

#endif /*IEICANPORT_HPP_*/
