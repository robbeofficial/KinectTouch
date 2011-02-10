/*
 TUIO Server Component - part of the reacTIVision project
 http://reactivision.sourceforge.net/
 
 Copyright (c) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_TuioServer_H
#define INCLUDED_TuioServer_H

#ifndef WIN32
#include <pthread.h>
#include <sys/time.h>
#define DllImport
#define DllExport
#else
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
#include <windows.h>
#endif

#include <iostream>
#include <list>
#include <algorithm>

#include "osc/OscOutboundPacketStream.h"
#include "ip/NetworkingUtils.h"
#include "ip/UdpSocket.h"

#include "TuioObject.h"
#include "TuioCursor.h"

#define IP_MTU_SIZE 1500
#define MAX_UDP_SIZE 65536
#define MIN_UDP_SIZE 576
#define OBJ_MESSAGE_SIZE 108	// setMessage + seqMessage size
#define CUR_MESSAGE_SIZE 88 // TODO think about it!!

namespace TUIO {
	/**
	 * <p>The TuioServer class is the central TUIO protocol encoder component.
	 * In order to encode and send TUIO messages an instance of TuioServer needs to be created. The TuioServer instance then generates TUIO messaged
	 * which are sent via OSC over UDP to the configured IP address and port.</p> 
	 * <p>During runtime the each frame is marked with the initFrame and commitFrame methods, 
	 * while the currently present TuioObjects are managed by the server with ADD, UPDATE and REMOVE methods in analogy to the TuioClient's TuioListener interface.</p> 
	 * <p><code>
	 * TuioClient *server = new TuioServer();<br/>
	 * ...<br/>
	 * server->initFrame(TuioTime::getSessionTime());<br/>
	 * TuioObject *tobj = server->addTuioObject(xpos,ypos, angle);<br/>
	 * TuioCursor *tcur = server->addTuioObject(xpos,ypos);<br/>
	 * server->commitFrame();<br/>
	 * ...<br/>
	 * server->initFrame(TuioTime::getSessionTime());<br/>
	 * server->updateTuioObject(tobj, xpos,ypos, angle);<br/>
	 * server->updateTuioCursor(tcur, xpos,ypos);<br/>
	 * server->commitFrame();<br/>
	 * ...<br/>
	 * server->initFrame(TuioTime::getSessionTime());<br/>
	 * server->removeTuioObject(tobj);<br/>
	 * server->removeTuioCursor(tcur);<br/>
	 * server->commitFrame();<br/>
	 * </code></p>
	 *
	 * @author Martin Kaltenbrunner
	 * @version 1.4
	 */ 
	class TuioServer { 
		
	public:

		/**
		 * The default constructor creates a TuioServer that sends to the default TUIO port 3333 on localhost
		 * using the maximum packet size of 65536 bytes to use single packets on the loopback device
		 */
		DllExport TuioServer(bool mode3d = false);

		/**
		 * This constructor creates a TuioServer that sends to the provided port on the the given host
		 * using a default packet size of 1492 bytes to deliver unfragmented UDP packets on a LAN
		 *
		 * @param  host  the receiving host name
		 * @param  port  the outgoing TUIO UDP port number
		 */
		DllExport TuioServer(const char *host, int port, bool mode3d = false);

		/**
		 * This constructor creates a TuioServer that sends to the provided port on the the given host
		 * the packet UDP size can be set to a value between 576 and 65536 bytes
		 *
		 * @param  host  the receiving host name
		 * @param  port  the outgoing TUIO UDP port number
		 * @param  size  the maximum UDP packet size
		 */
		DllExport TuioServer(const char *host, int port, int size, bool mode3d = false);

		/**
		 * The destructor is doing nothing in particular. 
		 */
		DllExport ~TuioServer();
		
		/**
		 * Creates a new TuioObject based on the given arguments.
		 * The new TuioObject is added to the TuioServer's internal list of active TuioObjects 
		 * and a reference is returned to the caller.
		 *
		 * @param	sym	the Symbol ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	a	the angle to assign
		 * @return	reference to the created TuioObject
		 */
		DllExport TuioObject* addTuioObject(int sym, float xp, float yp, float a);

		/**
		 * Updates the referenced TuioObject based on the given arguments.
		 *
		 * @param	tobj	the TuioObject to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	a	the angle to assign
		 */
		DllExport void updateTuioObject(TuioObject *tobj, float xp, float yp, float a);

		/**
		 * Removes the referenced TuioObject from the TuioServer's internal list of TuioObjects
		 * and deletes the referenced TuioObject afterwards
		 *
		 * @param	tobj	the TuioObject to remove
		 */
		DllExport void removeTuioObject(TuioObject *tobj);

		/**
		 * Adds an externally managed TuioObject to the TuioServer's internal list of active TuioObjects 
		 *
		 * @param	tobj	the TuioObject to add
		 */
		DllExport void addExternalTuioObject(TuioObject *tobj);

		/**
		 * Updates an externally managed TuioObject 
		 *
		 * @param	tobj	the TuioObject to update
		 */
		DllExport void updateExternalTuioObject(TuioObject *tobj);

		/**
		 * Removes an externally managed TuioObject from the TuioServer's internal list of TuioObjects
		 * The referenced TuioObject is not deleted
		 *
		 * @param	tobj	the TuioObject to remove
		 */
		DllExport void removeExternalTuioObject(TuioObject *tobj);
		
		/**
		 * Creates a new TuioCursor based on the given arguments.
		 * The new TuioCursor is added to the TuioServer's internal list of active TuioCursors 
		 * and a reference is returned to the caller.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @return	reference to the created TuioCursor
		 */
		DllExport TuioCursor* addTuioCursor(float xp, float yp, float zp=0);

		/**
		 * Updates the referenced TuioCursor based on the given arguments.
		 *
		 * @param	tcur	the TuioObject to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 */
		DllExport void updateTuioCursor(TuioCursor *tcur, float xp, float yp, float zp=0);

		/**
		 * Removes the referenced TuioCursor from the TuioServer's internal list of TuioCursors
		 * and deletes the referenced TuioCursor afterwards
		 *
		 * @param	tcur	the TuioCursor to remove
		 */
		DllExport void removeTuioCursor(TuioCursor *tcur);

		/**
		 * Updates an externally managed TuioCursor 
		 *
		 * @param	tcur	the TuioCursor to update
		 */
		DllExport void addExternalTuioCursor(TuioCursor *tcur);

		/**
		 * Updates an externally managed TuioCursor 
		 *
		 * @param	tcur	the TuioCursor to update
		 */
		DllExport void updateExternalTuioCursor(TuioCursor *tcur);

		/**
		 * Removes an externally managed TuioCursor from the TuioServer's internal list of TuioCursor
		 * The referenced TuioCursor is not deleted
		 *
		 * @param	tcur	the TuioCursor to remove
		 */
		DllExport void removeExternalTuioCursor(TuioCursor *tcur);
		
		/**
		 * Initializes a new frame with the given TuioTime
		 *
		 * @param	ttime	the frame time
		 */
		DllExport void initFrame(TuioTime ttime);
		
		/**
		 * Commits the current frame.
		 * Generates and sends TUIO messages of all currently active and updated TuioObjects and TuioCursors.
		 */
		DllExport void commitFrame();

		/**
		 * Returns the next available Session ID for external use.
		 * @return	the next available Session ID for external use
		 */
		DllExport long getSessionID();

		/**
		 * Returns the current frame ID for external use.
		 * @return	the current frame ID for external use
		 */
		DllExport long getFrameID();
		
		/**
		 * Returns the current frame ID for external use.
		 * @return	the current frame ID for external use
		 */
		DllExport TuioTime getFrameTime();

		/**
		 * Generates and sends TUIO messages of all currently active TuioObjects and TuioCursors.
		 */
		DllExport void sendFullMessages();		

		/**
		 * Disables the periodic full update of all currently active TuioObjects and TuioCursors 
		 *
		 * @param	interval	update interval in seconds, defaults to one second
		 */
		DllExport void enablePeriodicMessages(int interval=1);

		/**
		 * Disables the periodic full update of all currently active and inactive TuioObjects and TuioCursors 
		 */
		DllExport void disablePeriodicMessages();

		/**
		 * Enables the full update of all currently active and inactive TuioObjects and TuioCursors 
		 *
		 */
		DllExport void enableFullUpdate()  {
			full_update = true;
		}
		
		/**
		 * Disables the full update of all currently active and inactive TuioObjects and TuioCursors 
		 */
		DllExport void disableFullUpdate() {
			full_update = false;
		}
		
		/**
		 * Returns true if the periodic full update of all currently active TuioObjects and TuioCursors is enabled.
		 * @return	true if the periodic full update of all currently active TuioObjects and TuioCursors is enabled
		 */
		DllExport bool periodicMessagesEnabled() {
			return periodic_update;
		}
	
		/**
		 * Returns the periodic update interval in seconds.
		 * @return	the periodic update interval in seconds
		 */
		DllExport int getUpdateInterval() {
			return update_interval;
		}
		
		/**
		 * Returns a List of all currently inactive TuioObjects
		 *
		 * @return  a List of all currently inactive TuioObjects
		 */
		DllExport std::list<TuioObject*> getUntouchedObjects();

		/**
		 * Returns a List of all currently inactive TuioCursors
		 *
		 * @return  a List of all currently inactive TuioCursors
		 */
		DllExport std::list<TuioCursor*> getUntouchedCursors();
		
		/**
		 * Calculates speed and acceleration values for all currently inactive TuioObjects
		 */
		DllExport void stopUntouchedMovingObjects();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioCursors
		 */
		DllExport void stopUntouchedMovingCursors();
		
		/**
		 * Removes all currently inactive TuioObjects from the TuioServer's internal list of TuioObjects
		 */
		DllExport void removeUntouchedStoppedObjects();

		/**
		 * Removes all currently inactive TuioCursors from the TuioServer's internal list of TuioCursors
		 */
		DllExport void removeUntouchedStoppedCursors();

		/**
		 * Returns a List of all currently active TuioObjects
		 *
		 * @return  a List of all currently active TuioObjects
		 */
		DllExport std::list<TuioObject*> getTuioObjects();
		
		
		/**
		 * Returns a List of all currently active TuioCursors
		 *
		 * @return  a List of all currently active TuioCursors
		 */
		DllExport std::list<TuioCursor*> getTuioCursors();
		
		/**
		 * Returns the TuioObject corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioObject
		 *
		 * @return  an active TuioObject corresponding to the provided Session ID or NULL
		 */
		DllExport TuioObject* getTuioObject(long s_id);
		
		/**
		 * Returns the TuioCursor corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioCursor
		 *
		 * @return  an active TuioCursor corresponding to the provided Session ID or NULL
		 */
		DllExport TuioCursor* getTuioCursor(long s_id);

		/**
		 * Returns the TuioObject closest to the provided coordinates
		 * or NULL if there isn't any active TuioObject
		 *
		 * @return  the closest TuioObject to the provided coordinates or NULL
		 */
		DllExport TuioObject* getClosestTuioObject(float xp, float yp);
		
		/**
		 * Returns the TuioCursor closest to the provided coordinates
		 * or NULL if there isn't any active TuioCursor
		 *
		 * @return  the closest TuioCursor corresponding to the provided coordinates or NULL
		 */
		DllExport TuioCursor* getClosestTuioCursor(float xp, float yp, float zp=0);
		
		/**
		 * Returns true if this TuioServer is currently connected.
		 * @return	true if this TuioServer is currently connected
		 */
		DllExport bool isConnected() { return connected; }
		
		/**
		 * The TuioServer prints verbose TUIO event messages to the console if set to true.
		 * @param	verbose	verbose message output if set to true
		 */
		DllExport void setVerbose(bool verbose) { this->verbose=verbose; }

		//void set3d(bool mode3d) { this->mode3d=mode3d; }
		DllExport bool isMode3d() { return mode3d; }
		
	private:
		std::list<TuioObject*> objectList;
		std::list<TuioCursor*> cursorList;
		
		int maxCursorID;
		std::list<TuioCursor*> freeCursorList;
		std::list<TuioCursor*> freeCursorBuffer;
		
		UdpTransmitSocket *socket;	
		osc::OutboundPacketStream  *oscPacket;
		char *oscBuffer; 
		osc::OutboundPacketStream  *fullPacket;
		char *fullBuffer; 
		
		void initialize(const char *host, int port, int size, bool mode3d = false);

		void sendEmptyCursorBundle();
		void startCursorBundle();
		void addCursorMessage(TuioCursor *tcur);
		void sendCursorBundle(long fseq);
		
		void sendEmptyObjectBundle();
		void startObjectBundle();
		void addObjectMessage(TuioObject *tobj);
		void sendObjectBundle(long fseq);
		
		bool full_update;
		int update_interval;
		bool periodic_update;

		long currentFrame;
		TuioTime currentFrameTime;
		bool updateObject, updateCursor;
		long lastCursorUpdate, lastObjectUpdate;

		long sessionID;
		bool verbose;

		bool mode3d;
		char* cursorMessage;

#ifndef WIN32
		pthread_t thread;
#else
		HANDLE thread;
#endif	
		bool connected;
	};
};
#endif /* INCLUDED_TuioServer_H */
