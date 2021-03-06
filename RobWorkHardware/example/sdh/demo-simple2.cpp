//======================================================================
/*!
    \file
     \section sdhlibrary_cpp_sdh_demo_simple_general General file information

       \author   Dirk Osswald
       \date     2007-01-18

     \brief
       Very simple C++ programm to make the SDH move. With non-sequential call of move and Stop.

       This code contains only the very basicst use of the features
       provided by the SDHLibrary-CPP. For more sophisticated
       applications see the other demo-*.cpp programms, or of course
       the html/pdf documentation.

     \section sdhlibrary_cpp_sdh_demo_simple_copyright Copyright

     Copyright (c) 2007 SCHUNK GmbH & Co. KG

     <HR>
     \internal

       \subsection sdhlibrary_cpp_sdh_demo_simple_details SVN related, detailed file specific information:
         $LastChangedBy: Osswald2 $
         $LastChangedDate: 2008-10-13 17:07:24 +0200 (Mo, 13 Okt 2008) $
         \par SVN file revision:
           $Id: demo-simple2.cpp 3686 2008-10-13 15:07:24Z Osswald2 $

     \subsection sdhlibrary_cpp_sdh_demo_simple_changelog Changelog of this file:
         \include demo-simple2.cpp.log

*/
//======================================================================

#include <iostream>
#include <vector>


// Include the cSDH interface
#include "sdh/sdh.h"
#include "sdh/util.h"
#include "sdh/sdhlibrary_settings.h"
#include "sdh/basisdef.h"
#include "sdhoptions.h"

USING_NAMESPACE_SDH

/*!
    \anchor sdhlibrary_cpp_demo_simple2_cpp_vars
    \name   Some informative variables

    @{
*/
char const* __help__      = "Move proximal and distal joints of finger 1 three times by 10 degrees, stop movement when halfway done.\n(C++ demo application using the SDHLibrary-CPP library.)";
char const* __author__    = "Dirk Osswald: dirk.osswald@de.schunk.com";
char const* __url__       = "http://www.schunk.com";
char const* __version__   = "$Id: demo-simple2.cpp 3686 2008-10-13 15:07:24Z Osswald2 $";
char const* __copyright__ = "Copyright (c) 2007 SCHUNK GmbH & Co. KG";

//  end of doxygen name group sdhlibrary_cpp_demo_simple2_cpp_vars
//  @}



int main( int argc, char** argv )
{
    SDH_ASSERT_TYPESIZES();

    //---------------------
    // handle command line options: set defaults first then overwrite by parsing actual command line
    cSDHOptions options;

    options.Parse( argc, argv, __help__, "demo-simple2", __version__, cSDH::GetLibraryName(), cSDH::GetLibraryRelease() );
    //
    //---------------------

    //---------------------
    // initialize debug message printing:
    cDBG cdbg( options.debug_level > 0, "red", options.debuglog );
    g_sdh_debug_log = options.debuglog;

    cdbg << "Debug messages of " << argv[0] << " are printed like this.\n";

    // reduce debug level for subsystems
    options.debug_level-=1;
    //---------------------

    int    iFinger        = 0;     // The index of the finger to move

    try
    {
        // Create an instance "hand" of the class cSDH:
        cSDH hand( options.use_radians, options.use_fahrenheit, options.debug_level );

        if ( options.use_can )
            hand.OpenCAN_ESD( options.net,          // ESD CAN net number
                              options.can_baudrate, // CAN baudrate in bit/s
                              options.timeout,      // timeout in s
                              options.id_read,      // CAN ID used for reading
                              options.id_write );   // CAN ID used for writing
        else
            // Open communication to the SDH device via default serial port 0 == "COM1"
            hand.OpenRS232( options.port, options.rs232_baudrate, options.timeout );

        // save current velocity profile and set velocity profile to ramp
        cSDH::eVelocityProfile old_profile = hand.GetVelocityProfile();
        hand.SetVelocityProfile( cSDH::eVP_RAMP );

        // Now perform some action:
        //   get the current actual axis angles of finger iFinger:
        std::vector<double> faa = hand.GetFingerActualAngle( iFinger );

        //   sometimes the actual angles are reported slightly out of range
        //   (Like -0.001 for axis 0 ). So limit the angles to the allowed range:
        ToRange( faa, hand.GetFingerMinAngle( iFinger ), hand.GetFingerMaxAngle( iFinger ) );


        //   modify faa by decrementing the proximal and the distal axis angles
        //   (make a copy fta of faa and modify that to keep actual pose available)
        std::vector<double> fta = faa;

        fta[1] -= 40.0;
        fta[2] -= 40.0;

        //   keep fta in range too:
        ToRange( fta, hand.GetFingerMinAngle( iFinger ), hand.GetFingerMaxAngle( iFinger ) );

        std::cout << "Moving finger " << iFinger << " between faa=" << faa << " and fta=" << fta << "\n";

        //   now move for 3 times between these two poses:
        for (int i=0; i<3; i++ )
        {
            // set a new target angles
            hand.SetFingerTargetAngle( iFinger, fta );

            // and make the finger move there, but call non-sequentially (i.e. return immediately):
            double t = hand.MoveFinger( iFinger, false );
            // The last call returned immediately, the finger is now moving for t seconds

            // So wait until that movement is half finished:
            SleepSec( t/2 );

            // Then stop the finger in the middle of the movement:
            // (necessary for velocity profile "sin square" not for "ramp"
            //hand.Stop();


            // set a new target angles
            hand.SetFingerTargetAngle( iFinger, faa );

            // and make the finger move there:
            hand.MoveFinger( iFinger );

        }

        // restore previous velocity profile
        hand.SetVelocityProfile( old_profile );

        // Finally close connection to SDH again, this switches the axis controllers off
        hand.Close();
    }
    catch (cSDHLibraryException* e)
    {
        std::cerr << "demo-simple2 main(): An exception was caught: " << e->what() << "\n";
        delete e;
    }
}
//----------------------------------------------------------------------


//======================================================================
/*
  Here are some settings for the emacs/xemacs editor (and can be safely ignored)
  (e.g. to explicitely set C++ mode for *.h header files)

  Local Variables:
  mode:C++
  mode:ELSE
  End:
*/
//======================================================================]
